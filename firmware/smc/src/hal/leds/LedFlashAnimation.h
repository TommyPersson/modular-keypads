#pragma once

#include <stdint.h>

namespace smc::hal::leds {
    /**
     * The red level of an identification flash, the same as the pulse animation in the main firmware:
     * a 500 ms triangle wave, for 3 seconds. The level only changes every STEP_US, as every LED refresh
     * disables interrupts and can corrupt a concurrent SPI transaction.
     */
    class LedFlashAnimation {
    public:
        /**
         * Starts the animation, or restarts it when already active.
         */
        void start();

        /**
         * Advances the animation. Returns false when it isn't active (anymore).
         */
        bool update(uint32_t deltaUs);

        uint8_t level() const;

    private:
        static constexpr uint32_t DURATION_US = 3'000'000;
        static constexpr uint32_t PERIOD_US = 500'000;
        static constexpr uint32_t STEP_US = 20'000;
        // Peak red level, kept low to limit the current draw of all LEDs lighting up at once
        static constexpr uint8_t MAX_LEVEL = 64;

        bool isActive = false;
        uint32_t elapsedUs = 0;
    };
}
