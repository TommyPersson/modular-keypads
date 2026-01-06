#include "IndicatorLed.h"

#include <utils/time/Time.h>

IndicatorLed::IndicatorLed(IndicatorLedDriver& driver, const uint8_t pixelIndex) :
    driver(driver),
    pixelIndex(pixelIndex),
    color(0),
    overriddenColor(0) {
}

IndicatorLed::IndicatorLed(const IndicatorLed& other) = default;

IndicatorLed::~IndicatorLed() = default;

void IndicatorLed::update() {
    if (currentAnimation) {
        const auto now = utils::time::millis();
        const auto animatedColor = currentAnimation->getColor(now);

        color = animatedColor;
        driver.setPixelColor(pixelIndex, color);

        if (currentAnimation->isFinished(now)) {
            currentAnimation.reset();
            setColor(overriddenColor);
        }
    }
}

void IndicatorLed::setColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) {
    setColor(driver.makeColor(r, g, b, w));
}

void IndicatorLed::setColor(const uint32_t _color) {
    this->overriddenColor = _color;

    if (this->color == _color || currentAnimation) {
        return;
    }

    this->color = _color;

    driver.setPixelColor(pixelIndex, color);
}

uint32_t IndicatorLed::getColor() const {
    return color;
}

uint32_t IndicatorLed::makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return driver.makeColor(r, g, b, w);
}


void IndicatorLed::animate(const std::shared_ptr<utils::led::animations::Animation>& animation) {
    currentAnimation = animation;
}
