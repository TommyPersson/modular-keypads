#include "IndicatorLed.h"

#include "IndicatorLedManager.h"

IndicatorLed::IndicatorLed(Adafruit_NeoPixel& neoPixel, const uint8_t pixelIndex, IndicatorLedManager& ledManager) :
    neoPixel(neoPixel),
    ledManager(ledManager),
    pixelIndex(pixelIndex),
    color(0) {
}

IndicatorLed::IndicatorLed(const IndicatorLed& other) = default;

IndicatorLed::~IndicatorLed() = default;

void IndicatorLed::setColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) {
    setColor(Adafruit_NeoPixel::Color(r, g, b, w));
}

void IndicatorLed::setColor(const uint32_t _color) {
    if (this->color == _color) {
        return;
    }

    this->color = _color;

    neoPixel.setPixelColor(pixelIndex, color);

    ledManager.markAsDirty();
}

uint32_t IndicatorLed::getColor() const {
    return color;
}
