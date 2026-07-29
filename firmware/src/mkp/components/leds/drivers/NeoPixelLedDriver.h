#pragma once

#include "../IndicatorLedDriver.h"

class NeoPixelLedDriver final : public IndicatorLedDriver {
public:
    NeoPixelLedDriver(
        uint16_t numberOfPixels,
        int16_t pin,
        neoPixelType type
    );
    ~NeoPixelLedDriver() override;

    void begin() override;
    void loop() override;
    void show() override;
    void setPixelColor(uint8_t uint8, uint32_t color) override;
    uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) override;

private:
    Adafruit_NeoPixel neoPixel;
    bool isDirty = true;
};