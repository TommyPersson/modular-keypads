#pragma once

#include <stdint.h>

#include "IndicatorLeds.h"
#include "LedFlashAnimation.h"
#include "hal/base/Component.h"

namespace smc::hal::leds {
    /**
     * Pulses all LEDs red, see LedFlashAnimation. Overrides the regular LED colors while active, so it
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
        IndicatorLeds& leds;
        LedFlashAnimation animation;
    };
}
