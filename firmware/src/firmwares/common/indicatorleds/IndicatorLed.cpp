#include "IndicatorLed.h"

IndicatorLed::IndicatorLed(IndicatorLedDriver& driver, const uint8_t pixelIndex) :
    driver(driver),
    pixelIndex(pixelIndex),
    color(0) {
}

IndicatorLed::IndicatorLed(const IndicatorLed& other) = default;

IndicatorLed::~IndicatorLed() = default;

void IndicatorLed::setColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) {
    setColor(driver.makeColor(r, g, b, w));
}

void IndicatorLed::setColor(const uint32_t _color) {
    if (this->color == _color) {
        return;
    }

    this->color = _color;

    driver.setPixelColor(pixelIndex, color);
}

uint32_t IndicatorLed::getColor() const {
    return color;
}
