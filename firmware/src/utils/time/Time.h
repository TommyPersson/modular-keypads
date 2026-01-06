#pragma once

#ifdef ESP32
#include <esp32-hal.h>
#endif

namespace utils::time {
    inline uint64_t micros() {
#ifdef ESP32
        return ::micros();
#endif
    }

    inline uint64_t millis() {
#ifdef ESP32
        return ::millis();
#endif
    }

    inline void delayMs(const uint32_t ms) {
#ifdef ESP32
        ::delay(ms);
#endif
    };
}
