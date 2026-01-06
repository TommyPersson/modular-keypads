#pragma once

#include "Animations.h"
#include "IndicatorLedDriver.h"

class IndicatorLedManager;

class IndicatorLed {
public:
    IndicatorLed(IndicatorLedDriver& driver, uint8_t pixelIndex);
    IndicatorLed(const IndicatorLed& other);
    ~IndicatorLed();

    void update();

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void setColor(uint32_t color);
    uint32_t getColor() const;

    uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    void animate(const std::shared_ptr<utils::led::animations::Animation>& animation);

private:
    IndicatorLedDriver& driver;

    uint8_t pixelIndex;
    uint32_t color;
    uint32_t overriddenColor;

    std::shared_ptr<utils::led::animations::Animation> currentAnimation;
};
