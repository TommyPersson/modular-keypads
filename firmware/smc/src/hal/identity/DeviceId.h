#pragma once

#include <stdint.h>

namespace smc::hal::identity {
    inline constexpr uint8_t DEVICE_ID_LENGTH = 8;

    /**
     * 64-bit device ID: FNV-1a over the 10-byte factory serial number (SIGROW.SERNUM0..9), least
     * significant byte first. The serial number is fixed, so the ID is the same on every startup.
     */
    void computeDeviceId(uint8_t (&id)[DEVICE_ID_LENGTH]);
}
