#pragma once

#include <stdint.h>
#include <tinyNeoPixel.h>

#include "hal/base/Component.h"

namespace smc::hal::leds {
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        bool operator==(const Color& other) const {
            return r == other.r && g == other.g && b == other.b;
        }

        bool operator!=(const Color& other) const {
            return !(*this == other);
        }
    };

    class IndicatorLeds : public Component {
    public:
        IndicatorLeds(uint8_t count, uint8_t pin);
        ~IndicatorLeds() override;

        IndicatorLeds(const IndicatorLeds&) = delete;
        IndicatorLeds& operator=(const IndicatorLeds&) = delete;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        void setColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 255);

    private:
        const uint8_t count;
        tinyNeoPixel pixels;
        Color* colors;
        bool dirty = true;
    };
}
