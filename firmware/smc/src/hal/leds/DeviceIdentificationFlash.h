#pragma once

#include <stdint.h>

#include "IndicatorLeds.h"
#include "hal/base/Component.h"

namespace smc::hal::leds {
    /**
     * Pulses all LEDs red, the same as the pulse animation in the main firmware: a 500 ms triangle
     * wave, for 3 seconds. The level only changes every STEP_US, as every LED refresh disables interrupts
     * and can corrupt a concurrent SPI transaction. Overrides the regular LED colors while active, so it
     * must be updated after the components that set them. When it ends, their next setColor() calls
     * restore the regular colors.
     */
    class DeviceIdentificationFlash : public Component {
    public:
        explicit DeviceIdentificationFlash(IndicatorLeds& leds);
        ~DeviceIdentificationFlash() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        void start();

    private:
        static constexpr uint32_t DURATION_US = 3'000'000;
        static constexpr uint32_t PERIOD_US = 500'000;
        static constexpr uint32_t STEP_US = 20'000;
        // Peak red level, kept low to limit the current draw of all LEDs lighting up at once
        static constexpr uint8_t MAX_LEVEL = 64;

        IndicatorLeds& leds;
        bool isActive = false;
        uint32_t elapsedUs = 0;
    };
}
