#pragma once

#include "../IndicatorLedDriver.h"

class NoOpLedDriver final : public IndicatorLedDriver {
public:
    NoOpLedDriver() = default;
    ~NoOpLedDriver() override = default;

    void begin() override {}
    void loop() override {}
    void show() override {}
    void setPixelColor(uint8_t uint8, uint32_t color) override {}
    uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) override { return 0; }
};