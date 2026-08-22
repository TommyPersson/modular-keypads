#include "IndicatorLedManager.h"

#include <tfw/hal/buttons/Button.h>

using namespace mkp::components::leds;

IndicatorLedManager::IndicatorLedManager(
    const uint16_t numberOfPixels,
    std::unique_ptr<IndicatorLedDriver> driver,
    tfw::hal::time::Clock& clock
    ) : driver(std::move(driver)), clock(clock) {

    for (int i = 0; i < numberOfPixels; i++) {
        leds.emplace_back(std::make_shared<IndicatorLed>(*this->driver, i, clock));
    }
}

IndicatorLedManager::~IndicatorLedManager() {
    switchIndicators.clear();
    leds.clear();
};

void IndicatorLedManager::begin() {
    driver->begin();
}

std::shared_ptr<SwitchIndicatorLed> IndicatorLedManager::connectToSwitch(
    const uint8_t pixelNumber,
    const tfw::hal::buttons::Button& switchMonitor
    ) {
    const auto& indicatorLed = get(pixelNumber);
    return switchIndicators.emplace_back(std::make_shared<SwitchIndicatorLed>(switchMonitor, *indicatorLed));
}

void IndicatorLedManager::update() {
    for (const auto& led : leds) {
        led->update();
    }

    driver->show();
}

std::shared_ptr<IndicatorLed>& IndicatorLedManager::get(const uint8_t pixelNumber) {
    return leds[pixelNumber];
}

std::unique_ptr<IndicatorLedManager> IndicatorLedManager::NeoPixel(
    uint16_t numberOfPixels,
    int16_t pin,
    tfw::hal::time::Clock& clock,
    neoPixelType type
    ) {
    return std::make_unique<IndicatorLedManager>(numberOfPixels, IndicatorLedDriver::NeoPixel(numberOfPixels, pin, type), clock);
}

std::unique_ptr<IndicatorLedManager> IndicatorLedManager::NoOp(uint16_t numberOfPixels, tfw::hal::time::Clock& clock) {
    return std::make_unique<IndicatorLedManager>(numberOfPixels, IndicatorLedDriver::NoOp(), clock);
}