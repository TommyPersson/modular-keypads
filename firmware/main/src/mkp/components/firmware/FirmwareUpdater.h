#pragma once

#include <cstdint>
#include <memory>
#include <span>

#include <tfw/hal/time/Clock.h>
#include <tfw/utils/result.h>

namespace mkp::components::firmware {
    /**
     * Writes a new application image, received in chunks, to the OTA partition that isn't running, and boots it.
     *
     * The image is the firmware.bin built by PlatformIO, not the merged image with the bootloader. It's only booted
     * once it has been completely written and verified, so a failed update leaves the running firmware in place.
     */
    class FirmwareUpdater {
    public:
        explicit FirmwareUpdater(const tfw::hal::time::Clock& clock);
        ~FirmwareUpdater();

        /// Starts a new update of the given size, discarding any unfinished one
        tfw::utils::void_result begin(uint32_t size);

        /// Chunks must be written in order, which the offset is checked against, as a lost chunk would corrupt the image
        tfw::utils::void_result write(uint32_t offset, std::span<const uint8_t> data);

        /// Verifies the image and makes it the one to boot, then restarts shortly, once the response has been sent
        tfw::utils::void_result finish();

        void abort();

        void loop();

    private:
        bool inProgress = false;
        uint32_t expectedSize = 0;
        uint32_t writtenSize = 0;

        // Updates that are left unfinished, e.g. when the config tool is closed, are discarded
        std::unique_ptr<tfw::hal::time::CountDown> idleTimeoutCountDown;
        std::unique_ptr<tfw::hal::time::CountDown> restartCountDown;
    };
}
