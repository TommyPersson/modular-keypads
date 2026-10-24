#include "FirmwareDrive.h"

#include <cinttypes>
#include <cstring>
#include <string>

#include <Arduino.h>
#include <esp_app_desc.h>
#include <firmware_msc_fat.h>

#include <tfw/hal/logging.h>

#include "mkp/firmwares/base/FirmwareVersion.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("FirmwareDrive");

    constexpr auto REPOSITORY_URL = "https://github.com/TommyPersson/modular-keypads";

    // Short and upper case, as FAT volume labels are
    constexpr auto VOLUME_LABEL = "KEYPADS";

    // As many clusters as FAT12 allows, with room to spare, as hosts tell FAT12 and FAT16 apart by the cluster count.
    // The drive is larger than the OTA partition, so that the firmware fits beside the readme and anything else the
    // host writes, like Windows' System Volume Information, and so that a failed copy can be followed by another.
    constexpr uint32_t CLUSTER_COUNT = 4000;

    // More than the 16 entries of the Arduino core's helpers, as long file names take several entries each
    constexpr uint32_t ROOT_DIRECTORY_ENTRIES = 64;
    constexpr uint32_t ROOT_DIRECTORY_SECTORS = ROOT_DIRECTORY_ENTRIES * sizeof(fat_dir_entry_t) / DISK_SECTOR_SIZE;

    // The first cluster of the data area is number 2
    constexpr uint32_t FIRST_CLUSTER = 2;

    // See https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/firmware-image-format.html
    constexpr uint8_t IMAGE_MAGIC = 0xE9;
    constexpr uint32_t IMAGE_CHIP_ID_OFFSET = 12;
    constexpr uint16_t IMAGE_CHIP_ID_ESP32S3 = 0x0009;
    constexpr uint32_t IMAGE_APP_DESCRIPTION_OFFSET = 32;

    std::string createReadme() {
        std::string readme;
        readme += "Modular Keypads\r\n";
        readme += "===============\r\n";
        readme += "\r\n";
        readme += "Firmware version: ";
        readme += mkp::firmwares::base::firmwareVersion();
        readme += "\r\n";
        readme += "\r\n";
        readme += "Source code, documentation and the config tool:\r\n";
        readme += REPOSITORY_URL;
        readme += "\r\n";
        readme += "\r\n";
        readme += "Updating the firmware\r\n";
        readme += "---------------------\r\n";
        readme += "\r\n";
        readme += "The keypad is in its drive mode, where it only shows this drive, so its keys don't work. The\r\n";
        readme += "firmware is normally updated with the config tool, and this drive is a backup for when that\r\n";
        readme += "isn't possible:\r\n";
        readme += "\r\n";
        readme += "1. Copy the new firmware.bin to this drive. It's the firmware.bin built by PlatformIO, not\r\n";
        readme += "   bootloader.bin or partitions.bin. The name of the copy doesn't matter.\r\n";
        readme += "2. The keypad restarts into the new firmware shortly after the copy has finished, and this drive\r\n";
        readme += "   disappears. The operating system may warn about the drive being removed without being ejected,\r\n";
        readme += "   which is harmless.\r\n";
        readme += "\r\n";
        readme += "A file that isn't a firmware for the keypads, or a damaged one, is ignored, and the current firmware\r\n";
        readme += "is kept. Files copied to this drive aren't kept, and are gone once the keypad restarts.\r\n";
        readme += "\r\n";
        readme += "To leave the drive mode without updating, press the boot button again, or unplug the keypad.\r\n";
        return readme;
    }

    std::string createRepositoryShortcut() {
        return std::string("[InternetShortcut]\r\nURL=") + REPOSITORY_URL + "\r\n";
    }

    bool isEsp32S3Application(const std::span<const uint8_t> data) {
        if (data.size() < IMAGE_APP_DESCRIPTION_OFFSET + sizeof(uint32_t) || data[0] != IMAGE_MAGIC) {
            return false;
        }

        uint16_t chipId;
        memcpy(&chipId, data.data() + IMAGE_CHIP_ID_OFFSET, sizeof(chipId));

        // Bootloaders are images too, but don't start with an application description
        uint32_t appDescriptionMagic;
        memcpy(&appDescriptionMagic, data.data() + IMAGE_APP_DESCRIPTION_OFFSET, sizeof(appDescriptionMagic));

        return chipId == IMAGE_CHIP_ID_ESP32S3 && appDescriptionMagic == ESP_APP_DESC_MAGIC_WORD;
    }

    uint32_t sectorsFor(const uint32_t size) {
        return (size + DISK_SECTOR_SIZE - 1) / DISK_SECTOR_SIZE;
    }
}

using namespace mkp::components::firmware;

FirmwareDrive* FirmwareDrive::instance = nullptr;

FirmwareDrive::FirmwareDrive(FirmwareUpdater& firmwareUpdater)
    : firmwareUpdater(firmwareUpdater) {
    instance = this;
}

FirmwareDrive::~FirmwareDrive() {
    msc.end();
    instance = nullptr;
}

void FirmwareDrive::setup() {
    const auto readme = createReadme();
    const auto repositoryShortcut = createRepositoryShortcut();
    const auto readmeSectors = sectorsFor(readme.size());
    const auto repositoryShortcutSectors = sectorsFor(repositoryShortcut.size());

    const auto tableSectors = fat_sectors_per_alloc_table(CLUSTER_COUNT + FIRST_CLUSTER, false);
    rootDirectoryLba = 1 + tableSectors;
    dataLba = rootDirectoryLba + ROOT_DIRECTORY_SECTORS;
    ramSectorCount = dataLba + readmeSectors + repositoryShortcutSectors;
    const auto totalSectors = dataLba + CLUSTER_COUNT;

    ramSectors = std::make_unique<uint8_t[]>(ramSectorCount * DISK_SECTOR_SIZE);
    const auto disk = ramSectors.get();
    memset(disk, 0, ramSectorCount * DISK_SECTOR_SIZE);

    const auto boot = fat_add_boot_sector(
        disk,
        totalSectors,
        tableSectors,
        fat_file_system_type(false),
        VOLUME_LABEL,
        static_cast<uint32_t>(ESP.getEfuseMac())
    );
    boot->max_root_dir_entries = ROOT_DIRECTORY_ENTRIES;
    fat_add_table(disk, boot, false);
    fat_add_label(disk, VOLUME_LABEL);

    const auto readmeCluster = FIRST_CLUSTER;
    const auto readmeEntry = fat_add_root_file(disk, 1, "README", "TXT", readme.size(), readmeCluster, false);
    readmeEntry->file_attr |= FAT_FILE_ATTR_READ_ONLY;
    memcpy(disk + (dataLba + readmeCluster - FIRST_CLUSTER) * DISK_SECTOR_SIZE, readme.data(), readme.size());

    const auto shortcutCluster = readmeCluster + readmeSectors;
    const auto shortcutEntry = fat_add_root_file(
        disk,
        2,
        "GITHUB",
        "URL",
        repositoryShortcut.size(),
        shortcutCluster,
        false
    );
    shortcutEntry->file_attr |= FAT_FILE_ATTR_READ_ONLY;
    memcpy(
        disk + (dataLba + shortcutCluster - FIRST_CLUSTER) * DISK_SECTOR_SIZE,
        repositoryShortcut.data(),
        repositoryShortcut.size()
    );

    msc.vendorID("MKP");
    msc.productID("Modular Keypad");
    msc.productRevision("1.0");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.mediaPresent(true);
    msc.begin(totalSectors, DISK_SECTOR_SIZE);
}

void FirmwareDrive::loop() {
    const auto state = updateState.load();

    // Finished from the loop rather than the USB task, as verifying the image takes a while
    if (state == UpdateState::Received) {
        const auto result = firmwareUpdater.finish();
        updateState = result.has_error ? UpdateState::Failed : UpdateState::Finished;
    }

    if (state != loggedUpdateState) {
        loggedUpdateState = state;

        switch (state) {
        case UpdateState::Receiving:
            logger->info("Firmware is being copied to the drive");
            break;
        case UpdateState::Rejected:
            logger->warn("A file copied to the drive is an ESP32 image, but not a firmware for the keypads");
            break;
        case UpdateState::Failed:
            logger->error("Firmware copied to the drive was not written, after %" PRIu32 " bytes", receivedSize);
            break;
        default:
            break;
        }
    }
}

int32_t FirmwareDrive::read(const uint32_t lba, const uint32_t offset, void* buffer, const uint32_t size) {
    const auto start = lba * DISK_SECTOR_SIZE + offset;
    const auto end = start + size;
    const auto ramEnd = ramSectorCount * DISK_SECTOR_SIZE;

    // Data the drive doesn't keep, like copied files, reads as zeros
    memset(buffer, 0, size);
    if (start < ramEnd) {
        memcpy(buffer, ramSectors.get() + start, std::min(end, ramEnd) - start);
    }

    return static_cast<int32_t>(size);
}

// Writes can span several sectors, which are handled one at a time, as each can belong to another part of the drive
int32_t FirmwareDrive::write(const uint32_t lba, const uint32_t offset, uint8_t* buffer, const uint32_t size) {
    auto position = lba * DISK_SECTOR_SIZE + offset;
    const std::span<const uint8_t> data(buffer, size);

    for (uint32_t written = 0; written < size;) {
        const auto sectorOffset = position % DISK_SECTOR_SIZE;
        const auto length = std::min<uint32_t>(DISK_SECTOR_SIZE - sectorOffset, size - written);

        writeSector(position / DISK_SECTOR_SIZE, sectorOffset, data.subspan(written, length));

        written += length;
        position += length;
    }

    return static_cast<int32_t>(size);
}

void FirmwareDrive::writeSector(const uint32_t lba, const uint32_t offset, const std::span<const uint8_t> data) {
    if (lba < ramSectorCount) {
        memcpy(ramSectors.get() + lba * DISK_SECTOR_SIZE + offset, data.data(), data.size());
    }

    // The firmware can be written over the drive's own files, when they have been deleted
    if (lba >= dataLba) {
        writeFirmwareData(lba, offset, data);
    }

    if (lba >= rootDirectoryLba && lba < dataLba && updateState == UpdateState::Receiving) {
        findFirmwareFileSize();
        checkFirmwareReceived();
    }
}

void FirmwareDrive::writeFirmwareData(const uint32_t lba, const uint32_t offset, const std::span<const uint8_t> data) {
    const auto state = updateState.load();
    if (state == UpdateState::Received || state == UpdateState::Finished) {
        return;
    }

    const auto position = (lba - firmwareLba) * DISK_SECTOR_SIZE + offset;
    const auto isNextFirmwareData = state == UpdateState::Receiving && lba >= firmwareLba && position == receivedSize;

    // Data inside an image can look like the start of one, so only data elsewhere starts a new update
    if (!isNextFirmwareData) {
        if (offset != 0 || data.empty() || data[0] != IMAGE_MAGIC) {
            return;
        }

        if (!isEsp32S3Application(data)) {
            updateState = UpdateState::Rejected;
            return;
        }

        // The size is only known once the host has written the directory entry, which can be after the data
        if (firmwareUpdater.begin().has_error) {
            updateState = UpdateState::Failed;
            return;
        }

        firmwareLba = lba;
        firmwareFileSize = 0;
        receivedSize = 0;
        updateState = UpdateState::Receiving;

        // The host may have written the directory entry, with the size, before the data
        findFirmwareFileSize();
    }

    // Also fails when the update has been taken over by the config tool, or has timed out
    if (firmwareUpdater.write(receivedSize, data).has_error) {
        updateState = UpdateState::Failed;
        return;
    }

    receivedSize += data.size();
    checkFirmwareReceived();
}

void FirmwareDrive::findFirmwareFileSize() {
    const auto firmwareCluster = firmwareLba - dataLba + FIRST_CLUSTER;
    const auto entries = reinterpret_cast<const fat_dir_entry_t*>(ramSectors.get() + rootDirectoryLba * DISK_SECTOR_SIZE);

    for (uint32_t i = 0; i < ROOT_DIRECTORY_ENTRIES; i++) {
        const auto& entry = entries[i];

        if (entry.file_magic == 0x00) {
            break;
        }

        // Deleted entries, and long file names, volume labels and directories, which all have one of these attributes
        if (entry.file_magic == 0xE5 || (entry.file_attr & (FAT_FILE_ATTR_VOLUME_LABEL | FAT_FILE_ATTR_SUBDIRECTORY))) {
            continue;
        }

        if (entry.data_start_sector == firmwareCluster && entry.file_size > 0) {
            firmwareFileSize = entry.file_size;
            return;
        }
    }
}

void FirmwareDrive::checkFirmwareReceived() {
    if (updateState == UpdateState::Receiving && firmwareFileSize > 0 && receivedSize >= firmwareFileSize) {
        updateState = UpdateState::Received;
    }
}

int32_t FirmwareDrive::onRead(const uint32_t lba, const uint32_t offset, void* buffer, const uint32_t size) {
    return instance != nullptr ? instance->read(lba, offset, buffer, size) : -1;
}

int32_t FirmwareDrive::onWrite(const uint32_t lba, const uint32_t offset, uint8_t* buffer, const uint32_t size) {
    return instance != nullptr ? instance->write(lba, offset, buffer, size) : -1;
}
