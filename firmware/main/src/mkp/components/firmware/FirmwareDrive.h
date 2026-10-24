#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <span>

#include <USBMSC.h>

#include "FirmwareUpdater.h"

namespace mkp::components::firmware {
    /**
     * A USB drive with a readme, and a backup way to update the firmware for when the config tool can't be used: the
     * firmware.bin built by PlatformIO is copied to the drive, which hands it to the firmware updater as it's written.
     *
     * The drive is a FAT12 file system that only exists in RAM, apart from the files copied to it, which aren't kept.
     * Copied files are recognized as firmware by their content rather than their name, as hosts write the data of a
     * file and its directory entry in any order. Their data must be written in order, which hosts do when copying to an
     * empty drive. The size of the file is taken from its directory entry, to tell when it has all been written.
     *
     * Only one drive can exist, as the USB callbacks have no context.
     */
    class FirmwareDrive {
    public:
        explicit FirmwareDrive(FirmwareUpdater& firmwareUpdater);
        ~FirmwareDrive();

        /// Must be created before USB is started, and set up before or after
        void setup();

        void loop();

    private:
        enum class UpdateState {
            Idle,
            Receiving,
            Received,
            Rejected,
            Failed,
            Finished,
        };

        int32_t read(uint32_t lba, uint32_t offset, void* buffer, uint32_t size);
        int32_t write(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t size);

        void writeSector(uint32_t lba, uint32_t offset, std::span<const uint8_t> data);
        void writeFirmwareData(uint32_t lba, uint32_t offset, std::span<const uint8_t> data);
        void findFirmwareFileSize();
        void checkFirmwareReceived();

        static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t size);
        static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t size);

    private:
        static FirmwareDrive* instance;

        FirmwareUpdater& firmwareUpdater;

        USBMSC msc;

        // The boot sector, the allocation table, the root directory and the files of the drive itself
        std::unique_ptr<uint8_t[]> ramSectors;
        uint32_t ramSectorCount = 0;
        uint32_t rootDirectoryLba = 0;
        uint32_t dataLba = 0;

        // Changed by the USB task, and read by the loop once the state says the update has stopped changing
        std::atomic<UpdateState> updateState = UpdateState::Idle;
        UpdateState loggedUpdateState = UpdateState::Idle;
        uint32_t firmwareLba = 0;
        uint32_t firmwareFileSize = 0;
        uint32_t receivedSize = 0;
    };
}
