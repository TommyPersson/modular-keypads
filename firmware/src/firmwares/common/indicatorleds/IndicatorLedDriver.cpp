#include "IndicatorLedDriver.h"

#include "drivers/NeoPixelLedDriver.h"
#include "drivers/NoOpLedDriver.h"

IndicatorLedDriver::IndicatorLedDriver() = default;

IndicatorLedDriver::~IndicatorLedDriver() = default;

std::unique_ptr<IndicatorLedDriver> IndicatorLedDriver::NeoPixel(
    uint16_t numberOfPixels,
    int16_t pin,
    neoPixelType type
    ) {
    return std::make_unique<NeoPixelLedDriver>(numberOfPixels, pin, type);
}

std::unique_ptr<IndicatorLedDriver> IndicatorLedDriver::NoOp() {
    return std::make_unique<NoOpLedDriver>();
}
