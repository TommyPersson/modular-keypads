#pragma once

#include <Adafruit_NeoPixel.h>
#include <memory>

class IndicatorLedDriver {
public:
    explicit IndicatorLedDriver();
    virtual ~IndicatorLedDriver();

    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual void show() = 0;

    virtual void setPixelColor(uint8_t uint8, uint32_t color) = 0;
    virtual uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) = 0;

    static std::unique_ptr<IndicatorLedDriver> NeoPixel(
        uint16_t numberOfPixels,
        int16_t pin,
        neoPixelType type
    );

    static std::unique_ptr<IndicatorLedDriver> NoOp();
};

