#pragma once

#include <cstdint>
#include <tfw/hal/time.h>

namespace tfw::hal::time::test {
    /**
     * Virtual clock implementation for testing.
     *
     * Tracks time in nanosecond precision internally to allow fine-grained
     * timing validation, but presents microsecond and millisecond interfaces.
     *
     * Allows tests to:
     * - Advance time programmatically
     * - Control delay behavior (can advance time or verify delays)
     * - Track all timing-related operations with nanosecond precision
     * - Verify timing sequences and constraints at hardware-relevant timescales
     *
     * Time advances only when explicitly advanced by the test or when delays are called.
     */
    class VirtualClock : public Clock {
    public:
        /**
         * Create a virtual clock starting at time 0.
         */
        VirtualClock() : _currentTimeNs(0) {
        }

        uint64_t micros() const override {
            return _currentTimeNs / 1000;
        }

        uint64_t millis() const override {
            return _currentTimeNs / 1000000;
        }

        void delayUs(uint32_t us) override {
            _currentTimeNs += static_cast<uint64_t>(us) * 1000;
        }

        void delayMs(uint32_t ms) override {
            _currentTimeNs += static_cast<uint64_t>(ms) * 1000000;
        }

        /**
         * Get the current time in nanoseconds.
         * Useful for fine-grained timing validation in tests.
         */
        uint64_t nanos() const override {
            return _currentTimeNs;
        }

        void delayNs(uint32_t ns) override {
            _currentTimeNs += ns;
        }

        /**
         * Advance the clock by the specified number of nanoseconds.
         * Useful for simulating precise timing between operations.
         */
        void advanceNs(uint64_t ns) {
            _currentTimeNs += ns;
        }

        /**
         * Advance the clock by the specified number of microseconds.
         * Useful for simulating the passage of time between operations.
         */
        void advanceUs(uint32_t us) {
            _currentTimeNs += static_cast<uint64_t>(us) * 1000;
        }

        /**
         * Advance the clock by the specified number of milliseconds.
         * Useful for simulating the passage of time between operations.
         */
        void advanceMs(uint32_t ms) {
            _currentTimeNs += static_cast<uint64_t>(ms) * 1000000;
        }

        /**
         * Set the clock to an absolute time in nanoseconds.
         * Useful for testing specific timing scenarios.
         */
        void setTimeNs(uint64_t ns) {
            _currentTimeNs = ns;
        }

        /**
         * Set the clock to an absolute time in microseconds.
         * Useful for testing specific timing scenarios.
         */
        void setTimeUs(uint64_t us) {
            _currentTimeNs = us * 1000;
        }

        /**
         * Reset the clock to time 0.
         */
        void reset() {
            _currentTimeNs = 0;
        }

    private:
        uint64_t _currentTimeNs;  // Internal time in nanoseconds
    };
}
