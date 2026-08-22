#pragma once

#include "Clock.h"

#ifdef ESP32
#include <esp32-hal.h>
#endif

namespace tfw::hal::time {
    /**
     * Platform clock implementation using native hardware timers.
     *
     * On ESP32: Uses the built-in micros(), millis(), delay(), delayMicroseconds()
     * On other platforms: Returns 0 for time queries, no-ops for delays
     */
    class PlatformClock : public Clock {
    public:
        uint64_t micros() const override {
#ifdef ESP32
            return ::micros();
#else
            return 0;
#endif
        }

        uint64_t millis() const override {
#ifdef ESP32
            return ::millis();
#else
            return 0;
#endif
        }

        uint64_t nanos() const override {
            // Platform doesn't support nanosecond precision, convert from micros
            return micros() * 1000;
        }

        void delayNs(uint32_t ns) override {
            // Round up to microseconds (platform minimum resolution)
            if (ns > 0) {
                delayUs((ns + 999) / 1000);
            }
        }

        void delayUs(uint32_t us) override {
#ifdef ESP32
            ::delayMicroseconds(us);
#endif
        }

        void delayMs(uint32_t ms) override {
#ifdef ESP32
            ::delay(ms);
#endif
        }
    };
}
