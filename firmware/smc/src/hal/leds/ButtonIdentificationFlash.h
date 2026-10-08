#pragma once

#include <stdint.h>

#include "IndicatorLeds.h"
#include "LedFlashAnimation.h"
#include "hal/base/Component.h"

namespace smc::hal::leds {
    /**
     * Pulses the LED of a single button red, see LedFlashAnimation. Overrides the regular color of that LED
     * while active, so it must be updated after the components that set it. When it ends, their next
     * setColor() call restores the regular color.
     */
    class ButtonIdentificationFlash : public Component {
    public:
        explicit ButtonIdentificationFlash(IndicatorLeds& leds);
        ~ButtonIdentificationFlash() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        /**
         * Starts flashing the given LED, or restarts the flash when already active (also for another LED).
         */
        void start(uint8_t ledIndex);

    private:
        IndicatorLeds& leds;
        LedFlashAnimation animation;
        uint8_t ledIndex = 0;
    };
}
