#pragma once

#include <Adafruit_NeoPixel.h>

class IndicatorLedManager;

class IndicatorLed {
public:
    IndicatorLed(Adafruit_NeoPixel& neoPixel, uint8_t pixelIndex, IndicatorLedManager& ledManager);
    IndicatorLed(const IndicatorLed& other);
    ~IndicatorLed();

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void setColor(uint32_t color);
    uint32_t getColor() const;

private:
    Adafruit_NeoPixel& neoPixel;
    IndicatorLedManager& ledManager;

    uint8_t pixelIndex;
    uint32_t color;
};
