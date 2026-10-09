#pragma once

#include <stdint.h>

namespace smc::hal::identity {
    // Including the terminating null character, so names are at most DEVICE_NAME_LENGTH - 1 characters long
    inline constexpr uint8_t DEVICE_NAME_LENGTH = 16;

    inline constexpr char DEFAULT_DEVICE_NAME[] = "Unnamed";

    /**
     * Replaces names that are empty or not printable ASCII (e.g. unwritten or leftover EEPROM contents) with
     * DEFAULT_DEVICE_NAME, the same as the main firmware's devices, and null pads and terminates the name.
     */
    void normalizeDeviceName(uint8_t (&name)[DEVICE_NAME_LENGTH]);

    /**
     * Stores the device name at the start of the EEPROM.
     */
    class DeviceNameStorage {
    public:
        /**
         * Reads the stored name, normalized. Must not be called while update() is still writing.
         */
        void load(uint8_t (&name)[DEVICE_NAME_LENGTH]) const;

        /**
         * Schedules the name to be written by update(), replacing a previously scheduled one.
         */
        void store(const uint8_t (&name)[DEVICE_NAME_LENGTH]);

        /**
         * Writes at most one changed byte per call, and only when the NVM controller is idle: reading the EEPROM
         * while it is writing halts the CPU, interrupts included, which would break concurrent SPI transactions.
         *
         * Returns true on the call that finishes storing the scheduled name.
         */
        bool update();

    private:
        uint8_t pendingName[DEVICE_NAME_LENGTH] = {};
        uint8_t nextIndex = 0;
        bool isStoring = false;
    };
}
