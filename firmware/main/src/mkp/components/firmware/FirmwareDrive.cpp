#include "FirmwareDrive.h"

#include <cinttypes>
#include <cstring>
#include <string>

#include <Arduino.h>
#include <esp32-hal-tinyusb.h>
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

    constexpr uint8_t DELETED_ENTRY = 0xE5;
    constexpr uint8_t LONG_FILE_NAME_ATTRIBUTES = 0x0F;
    constexpr uint8_t LONG_FILE_NAME_LAST_ENTRY = 0x40;
    constexpr uint32_t LONG_FILE_NAME_ENTRY_CHARACTERS = 13;

    // Set in the otherwise reserved byte of short file names that are all lower case, instead of a long file name
    constexpr uint8_t LOWER_CASE_NAME = 0x08;
    constexpr uint8_t LOWER_CASE_EXTENSION = 0x10;

    // The error log has a long file name, as it doesn't fit 8.3, so it takes one entry for its long and one for its
    // short name, after the volume label, the readme and the repository shortcut
    constexpr auto ERROR_LOG_NAME = "error-log.txt";
    constexpr uint8_t ERROR_LOG_ENTRY = 4;

    // How long the host must have stopped writing before the copied files are looked at, as files are only complete
    // when the host is done with them
    constexpr uint64_t QUIET_US = 1'000'000;

    // How long a firmware that's being copied can go without being written to before the copy counts as stopped
    constexpr uint64_t STALLED_US = 3'000'000;

    // How long the drive is reported as removed, for the host to notice it when it checks on the drive
    constexpr uint64_t MEDIA_CHANGE_US = 2'000'000;

    // See https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/firmware-image-format.html
    constexpr uint8_t IMAGE_MAGIC = 0xE9;
    constexpr uint32_t IMAGE_CHIP_ID_OFFSET = 12;
    constexpr uint16_t IMAGE_CHIP_ID_ESP32S3 = 0x0009;
    constexpr uint32_t IMAGE_APP_DESCRIPTION_OFFSET = 32;

    constexpr auto NOT_A_FIRMWARE = "It isn't a firmware image.";
    constexpr auto NOT_AN_APPLICATION = "It's an ESP32 image, but not a firmware, e.g. bootloader.bin.";
    constexpr auto OTHER_CHIP = "It's built for another chip than the ESP32-S3 of the keypads.";
    constexpr auto NOT_STARTED = "The update couldn't be started.";
    constexpr auto NOT_WRITTEN = "It couldn't be written.";
    constexpr auto STOPPED = "The copy stopped before all of it was written, or it wasn't written in order.";
    constexpr auto NOT_VERIFIED = "It's damaged or incomplete, as it didn't pass verification.";

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
        readme += "A file that isn't a firmware for the keypads, or a damaged one, isn't used, and the current\r\n";
        readme += "firmware is kept. The drive then disappears for a moment, and comes back with an error-log.txt\r\n";
        readme += "that tells what went wrong. Files copied to this drive aren't kept.\r\n";
        readme += "\r\n";
        readme += "To leave the drive mode without updating, press the boot button again, or unplug the keypad.\r\n";
        return readme;
    }

    std::string createRepositoryShortcut() {
        return std::string("[InternetShortcut]\r\nURL=") + REPOSITORY_URL + "\r\n";
    }

    std::string createErrorLog(const std::string& fileName, const std::string& problem) {
        std::string errorLog;
        errorLog += "The firmware update failed, and the current firmware is kept.\r\n";
        errorLog += "\r\n";
        errorLog += "File: " + fileName + "\r\n";
        errorLog += "Problem: " + problem + "\r\n";
        errorLog += "\r\n";
        errorLog += "To try again, copy the firmware.bin built by PlatformIO to this drive, not bootloader.bin or\r\n";
        errorLog += "partitions.bin. See README.TXT for more.\r\n";
        return errorLog;
    }

    // Why an image that starts like an ESP32 image can't be used, if it can't
    const char* checkImage(const std::span<const uint8_t> data) {
        if (data.size() < IMAGE_APP_DESCRIPTION_OFFSET + sizeof(uint32_t)) {
            return NOT_A_FIRMWARE;
        }

        uint16_t chipId;
        memcpy(&chipId, data.data() + IMAGE_CHIP_ID_OFFSET, sizeof(chipId));
        if (chipId != IMAGE_CHIP_ID_ESP32S3) {
            return OTHER_CHIP;
        }

        // Bootloaders are images too, but don't start with an application description
        uint32_t appDescriptionMagic;
        memcpy(&appDescriptionMagic, data.data() + IMAGE_APP_DESCRIPTION_OFFSET, sizeof(appDescriptionMagic));
        if (appDescriptionMagic != ESP_APP_DESC_MAGIC_WORD) {
            return NOT_AN_APPLICATION;
        }

        return nullptr;
    }

    uint32_t sectorsFor(const uint32_t size) {
        return (size + DISK_SECTOR_SIZE - 1) / DISK_SECTOR_SIZE;
    }

    bool isLongFileName(const fat_dir_entry_t& entry) {
        return entry.file_attr == LONG_FILE_NAME_ATTRIBUTES;
    }

    // Not deleted entries, nor long file names, volume labels or directories, which all have one of these attributes
    bool isFile(const fat_dir_entry_t& entry) {
        return entry.file_magic != DELETED_ENTRY
            && (entry.file_attr & (FAT_FILE_ATTR_VOLUME_LABEL | FAT_FILE_ATTR_SUBDIRECTORY)) == 0;
    }

    // The files macOS adds next to copied files, and to folders it has shown, which aren't hidden on FAT drives. Other
    // names starting with a dot are ordinary files, like PlatformIO's .sconsign.dblite.
    bool isMacOsMetadata(const std::string& fileName) {
        return fileName.starts_with("._") || fileName == ".DS_Store";
    }

    std::string trimmed(const char* text, const size_t length, const bool isLowerCase) {
        std::string result(text, length);
        result.erase(result.find_last_not_of(' ') + 1);

        if (isLowerCase) {
            for (auto& character : result) {
                character = static_cast<char>(tolower(character));
            }
        }

        return result;
    }

    // The long file name of an entry is in the entries before it, the first part closest to it. Characters outside
    // ASCII are shown as question marks.
    std::string readFileName(const fat_dir_entry_t* entries, const uint32_t index) {
        std::string longName;
        for (auto i = index; i > 0 && isLongFileName(entries[i - 1]); i--) {
            const auto& part = reinterpret_cast<const fat_lfn_entry_t&>(entries[i - 1]);

            uint16_t characters[LONG_FILE_NAME_ENTRY_CHARACTERS];
            memcpy(characters, part.name0, sizeof(part.name0));
            memcpy(characters + 5, part.name1, sizeof(part.name1));
            memcpy(characters + 11, part.name2, sizeof(part.name2));

            for (const auto character : characters) {
                if (character == 0x0000 || character == 0xFFFF) {
                    return longName;
                }
                longName += character < 0x80 ? static_cast<char>(character) : '?';
            }

            if (part.seq_num & LONG_FILE_NAME_LAST_ENTRY) {
                break;
            }
        }

        if (!longName.empty()) {
            return longName;
        }

        const auto& entry = entries[index];
        const auto name = trimmed(entry.file_name, sizeof(entry.file_name), entry.reserved & LOWER_CASE_NAME);
        const auto extension = trimmed(
            entry.file_extension,
            sizeof(entry.file_extension),
            entry.reserved & LOWER_CASE_EXTENSION
        );

        return extension.empty() ? name : name + "." + extension;
    }

    // Only for names of exactly one entry's worth of characters, which saves terminating and padding them
    void addLongFileName(fat_dir_entry_t* entries, const uint8_t index, const char* longName) {
        auto& part = reinterpret_cast<fat_lfn_entry_t&>(entries[index - 1]);
        memset(&part, 0, sizeof(part));

        uint16_t characters[LONG_FILE_NAME_ENTRY_CHARACTERS];
        for (uint32_t i = 0; i < LONG_FILE_NAME_ENTRY_CHARACTERS; i++) {
            characters[i] = longName[i];
        }

        part.seq_num = LONG_FILE_NAME_LAST_ENTRY | 1;
        memcpy(part.name0, characters, sizeof(part.name0));
        memcpy(part.name1, characters + 5, sizeof(part.name1));
        memcpy(part.name2, characters + 11, sizeof(part.name2));
        part.attr = LONG_FILE_NAME_ATTRIBUTES;
        part.dos_checksum = fat_lfn_checksum(reinterpret_cast<const uint8_t*>(entries[index].file_name));
    }
}

using namespace mkp::components::firmware;

FirmwareDrive* FirmwareDrive::instance = nullptr;

FirmwareDrive::FirmwareDrive(FirmwareUpdater& firmwareUpdater, const tfw::hal::time::Clock& clock)
    : firmwareUpdater(firmwareUpdater),
      clock(clock) {
    instance = this;

    mediaChangeCountDown = clock.createCountDown(MEDIA_CHANGE_US, [this] {
        msc.mediaPresent(true);
    });
}

FirmwareDrive::~FirmwareDrive() {
    msc.end();
    instance = nullptr;
}

void FirmwareDrive::setup() {
    createFileSystem("");

    msc.vendorID("MKP");
    msc.productID("Modular Keypad");
    msc.productRevision("1.0");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.mediaPresent(true);
    msc.begin(dataLba + CLUSTER_COUNT, DISK_SECTOR_SIZE);
}

// The drive's own files are kept in RAM, at the start of the data area, and the rest of it is free
void FirmwareDrive::createFileSystem(const std::string& errorLog) {
    const auto readme = createReadme();
    const auto repositoryShortcut = createRepositoryShortcut();
    const auto readmeSectors = sectorsFor(readme.size());
    const auto repositoryShortcutSectors = sectorsFor(repositoryShortcut.size());
    const auto errorLogSectors = sectorsFor(errorLog.size());

    const auto tableSectors = fat_sectors_per_alloc_table(CLUSTER_COUNT + FIRST_CLUSTER, false);
    rootDirectoryLba = 1 + tableSectors;
    dataLba = rootDirectoryLba + ROOT_DIRECTORY_SECTORS;
    ramSectorCount = dataLba + readmeSectors + repositoryShortcutSectors + errorLogSectors;
    firstFreeCluster = FIRST_CLUSTER + ramSectorCount - dataLba;

    ramSectors = std::make_unique<uint8_t[]>(ramSectorCount * DISK_SECTOR_SIZE);
    const auto disk = ramSectors.get();
    memset(disk, 0, ramSectorCount * DISK_SECTOR_SIZE);

    const auto boot = fat_add_boot_sector(
        disk,
        dataLba + CLUSTER_COUNT,
        tableSectors,
        fat_file_system_type(false),
        VOLUME_LABEL,
        static_cast<uint32_t>(ESP.getEfuseMac())
    );
    boot->max_root_dir_entries = ROOT_DIRECTORY_ENTRIES;
    fat_add_table(disk, boot, false);
    fat_add_label(disk, VOLUME_LABEL);

    auto cluster = FIRST_CLUSTER;
    const auto addFile = [&](const uint8_t index, const char* name, const char* extension, const std::string& content) {
        const auto entry = fat_add_root_file(disk, index, name, extension, content.size(), cluster, false);
        entry->file_attr |= FAT_FILE_ATTR_READ_ONLY;
        memcpy(disk + (dataLba + cluster - FIRST_CLUSTER) * DISK_SECTOR_SIZE, content.data(), content.size());
        cluster += sectorsFor(content.size());
    };

    addFile(1, "README", "TXT", readme);
    addFile(2, "GITHUB", "URL", repositoryShortcut);

    if (!errorLog.empty()) {
        addFile(ERROR_LOG_ENTRY, "ERROR-~1", "TXT", errorLog);
        const auto entries = reinterpret_cast<fat_dir_entry_t*>(disk + rootDirectoryLba * DISK_SECTOR_SIZE);
        addLongFileName(entries, ERROR_LOG_ENTRY, ERROR_LOG_NAME);
    }

    updateState = UpdateState::Idle;
    firmwareLba = 0;
    firmwareFileSize = 0;
    receivedSize = 0;
    failureReason = nullptr;
    failureErrorCode = nullptr;
    isRootDirectoryChanged = false;
}

void FirmwareDrive::loop() {
    mediaChangeCountDown->update();
    if (mediaChangeCountDown->isRunning()) {
        // Set again for every check of the host, which clears it by reading it. Without it, the host is only told that
        // the drive isn't ready, rather than that it's gone.
        tud_msc_set_sense(0, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
        return;
    }

    // Finished from the loop rather than the USB task, as verifying the image takes a while
    if (updateState == UpdateState::Received) {
        const auto result = firmwareUpdater.finish();
        if (result.has_error) {
            fail(NOT_VERIFIED, result.error_code);
        } else {
            updateState = UpdateState::Finished;
        }
    }

    const auto state = updateState.load();
    // Written by the USB task, possibly after the time is read
    const auto now = clock.micros();
    const uint64_t lastWrite = lastWriteUs;
    const auto quietUs = now > lastWrite ? now - lastWrite : 0;

    if (state == UpdateState::Receiving && quietUs >= STALLED_US) {
        fail(STOPPED);
        return;
    }

    if (quietUs < QUIET_US) {
        return;
    }

    if (state == UpdateState::Failed) {
        showFailure();
    } else if (state == UpdateState::Idle && isRootDirectoryChanged.exchange(false)) {
        checkForOtherFiles();
    }
}

void FirmwareDrive::showFailure() {
    std::string problem = failureReason;
    if (failureErrorCode != nullptr) {
        problem += " (";
        problem += failureErrorCode;
        problem += ")";
    }

    if (receivedSize > 0) {
        char received[48];
        snprintf(received, sizeof(received), " %" PRIu32 " bytes were received.", receivedSize);
        problem += received;
    }

    std::string fileName;
    {
        std::lock_guard lock(mutex);
        fileName = findFileName(firmwareLba - dataLba + FIRST_CLUSTER);
    }

    showError(fileName, problem);
}

// Files that never looked like a firmware, which are only found once the host has written them
void FirmwareDrive::checkForOtherFiles() {
    std::string fileName;
    {
        std::lock_guard lock(mutex);

        const auto entries = reinterpret_cast<const fat_dir_entry_t*>(
            ramSectors.get() + rootDirectoryLba * DISK_SECTOR_SIZE
        );

        for (uint32_t i = 0; i < ROOT_DIRECTORY_ENTRIES && entries[i].file_magic != 0x00; i++) {
            const auto& entry = entries[i];

            // Empty files, the drive's own files, and the hidden files hosts add, like Windows' System Volume
            // Information
            const auto isIgnored = !isFile(entry)
                || entry.file_size == 0
                || entry.data_start_sector < firstFreeCluster
                || (entry.file_attr & (FAT_FILE_ATTR_HIDDEN | FAT_FILE_ATTR_SYSTEM)) != 0;
            if (isIgnored) {
                continue;
            }

            const auto name = readFileName(entries, i);
            if (!isMacOsMetadata(name)) {
                fileName = name;
                break;
            }
        }
    }

    if (!fileName.empty()) {
        showError(fileName, NOT_A_FIRMWARE);
    }
}

// Replaces the drive with one with the error log, and reports it as removed for a moment, as hosts cache the contents
// of drives
void FirmwareDrive::showError(const std::string& fileName, const std::string& problem) {
    logger->warn("Firmware update from the drive failed, %s: %s", fileName.c_str(), problem.c_str());

    msc.mediaPresent(false);
    firmwareUpdater.abort();

    {
        std::lock_guard lock(mutex);
        createFileSystem(createErrorLog(fileName, problem));
    }

    mediaChangeCountDown->start();
}

std::string FirmwareDrive::findFileName(const uint32_t cluster) const {
    const auto entries = reinterpret_cast<const fat_dir_entry_t*>(ramSectors.get() + rootDirectoryLba * DISK_SECTOR_SIZE);

    for (uint32_t i = 0; i < ROOT_DIRECTORY_ENTRIES && entries[i].file_magic != 0x00; i++) {
        if (isFile(entries[i]) && entries[i].data_start_sector == cluster) {
            return readFileName(entries, i);
        }
    }

    return "(unknown)";
}

int32_t FirmwareDrive::read(const uint32_t lba, const uint32_t offset, void* buffer, const uint32_t size) {
    std::lock_guard lock(mutex);

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
    std::lock_guard lock(mutex);

    auto position = lba * DISK_SECTOR_SIZE + offset;
    const std::span<const uint8_t> data(buffer, size);

    for (uint32_t written = 0; written < size;) {
        const auto sectorOffset = position % DISK_SECTOR_SIZE;
        const auto length = std::min<uint32_t>(DISK_SECTOR_SIZE - sectorOffset, size - written);

        writeSector(position / DISK_SECTOR_SIZE, sectorOffset, data.subspan(written, length));

        written += length;
        position += length;
    }

    lastWriteUs = clock.micros();

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

    if (lba >= rootDirectoryLba && lba < dataLba) {
        isRootDirectoryChanged = true;

        if (updateState == UpdateState::Receiving) {
            findFirmwareFileSize();
            checkFirmwareReceived();
        }
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

        firmwareLba = lba;
        firmwareFileSize = 0;
        receivedSize = 0;

        if (const auto problem = checkImage(data)) {
            fail(problem);
            return;
        }

        // The size is only known once the host has written the directory entry, which can be after the data
        if (const auto result = firmwareUpdater.begin(); result.has_error) {
            fail(NOT_STARTED, result.error_code);
            return;
        }

        updateState = UpdateState::Receiving;

        // The host may have written the directory entry, with the size, before the data
        findFirmwareFileSize();
    }

    // Also fails when the update has timed out
    if (const auto result = firmwareUpdater.write(receivedSize, data); result.has_error) {
        fail(NOT_WRITTEN, result.error_code);
        return;
    }

    receivedSize += data.size();
    checkFirmwareReceived();
}

void FirmwareDrive::fail(const char* reason, const char* errorCode) {
    failureReason = reason;
    failureErrorCode = errorCode;
    updateState = UpdateState::Failed;
}

void FirmwareDrive::findFirmwareFileSize() {
    const auto firmwareCluster = firmwareLba - dataLba + FIRST_CLUSTER;
    const auto entries = reinterpret_cast<const fat_dir_entry_t*>(ramSectors.get() + rootDirectoryLba * DISK_SECTOR_SIZE);

    for (uint32_t i = 0; i < ROOT_DIRECTORY_ENTRIES && entries[i].file_magic != 0x00; i++) {
        const auto& entry = entries[i];

        if (isFile(entry) && entry.data_start_sector == firmwareCluster && entry.file_size > 0) {
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
