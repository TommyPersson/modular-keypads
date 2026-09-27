#pragma once

#include "PushButton.h"
#include "hal/base/Component.h"
#include "hal/leds/IndicatorLeds.h"

namespace smc::hal::buttons {
    class PushButtonWithIndicator : public Component {
    public:
        PushButtonWithIndicator(
            uint8_t number,
            uint8_t pin,
            leds::IndicatorLeds& leds,
            uint8_t ledIndex
        );
        ~PushButtonWithIndicator() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

    private:
        static constexpr uint8_t LED_BRIGHTNESS_PRESSED = 50;
        static constexpr uint8_t LED_BRIGHTNESS_LONG_PRESSED = 200;

        PushButton button;
        leds::IndicatorLeds& leds;
        uint8_t ledIndex;
    };
}
