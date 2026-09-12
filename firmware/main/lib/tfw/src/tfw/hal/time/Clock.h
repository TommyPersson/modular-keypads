#pragma once

#include <cstdint>

namespace tfw::hal::time {
    /**
     * Abstract clock interface for time tracking and delays.
     *
     * Allows decoupling from platform-specific time implementations,
     * enabling:
     * - Virtual time in tests with programmatic advancement
     * - Timing verification in test scenarios with nanosecond precision
     * - Simulation of timing sequences
     */
    class Clock {
    public:
        virtual ~Clock() = default;

        /**
         * Get the current time in nanoseconds.
         * The epoch (zero point) is platform/implementation-defined.
         * Default implementation converts from micros() - override for better precision.
         */
        virtual uint64_t nanos() const {
            return micros() * 1000;
        }

        /**
         * Get the current time in microseconds.
         * The epoch (zero point) is platform/implementation-defined.
         */
        virtual uint64_t micros() const = 0;

        /**
         * Get the current time in milliseconds.
         * The epoch (zero point) is platform/implementation-defined.
         */
        virtual uint64_t millis() const = 0;

        /**
         * Delay for the specified number of nanoseconds.
         * In tests, this may advance virtual time instead of blocking.
         * On platform implementations, will round up to the minimum supported delay.
         */
        virtual void delayNs(uint32_t ns) {
            // Default: round up to microseconds
            if (ns > 0) {
                delayUs((ns + 999) / 1000);  // Round up to nearest microsecond
            }
        }

        /**
         * Delay for the specified number of microseconds.
         * In tests, this may advance virtual time instead of blocking.
         */
        virtual void delayUs(uint32_t us) = 0;

        /**
         * Delay for the specified number of milliseconds.
         * In tests, this may advance virtual time instead of blocking.
         */
        virtual void delayMs(uint32_t ms) = 0;
    };
}
