#pragma once

#include <tfw/utils/leds.h>
#include <tfw/hal/time.h>

#include "IndicatorLedDriver.h"

namespace mkp::components::leds {
    class IndicatorLedManager;

    class IndicatorLed {
    public:
        IndicatorLed(IndicatorLedDriver& driver, uint8_t pixelIndex, tfw::hal::time::Clock& clock);
        IndicatorLed(const IndicatorLed& other);
        ~IndicatorLed();

        void update();

        void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void setColor(uint32_t color);
        uint32_t getColor() const;

        uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

        void animate(const std::shared_ptr<tfw::utils::leds::animations::Animation>& animation);

    private:
        IndicatorLedDriver& driver;
        tfw::hal::time::Clock& clock;

        uint8_t pixelIndex;
        uint32_t color;
        uint32_t overriddenColor;

        std::shared_ptr<tfw::utils::leds::animations::Animation> currentAnimation;
    };
}