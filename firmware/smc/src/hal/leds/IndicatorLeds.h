#pragma once

#include <stdint.h>
#include <tinyNeoPixel.h>

#include "hal/base/Component.h"

namespace smc::hal::leds {
    class IndicatorLeds : public Component {
    public:
        IndicatorLeds(uint8_t count, uint8_t pin);
        ~IndicatorLeds() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        void setColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
        void setBrightness(uint8_t brightness);

    private:
        tinyNeoPixel pixels;
        bool dirty = true;
    };
}
