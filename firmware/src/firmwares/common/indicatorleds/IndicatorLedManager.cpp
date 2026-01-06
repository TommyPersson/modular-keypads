#include "IndicatorLedManager.h"

IndicatorLedManager::IndicatorLedManager(
    const uint16_t numberOfPixels,
    std::unique_ptr<IndicatorLedDriver> driver
    ) : driver(std::move(driver)) {

    for (int i = 0; i < numberOfPixels; i++) {
        leds.emplace_back(std::make_shared<IndicatorLed>(*this->driver, i));
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
    const SwitchMonitor& switchMonitor
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
    neoPixelType type
    ) {
    return std::make_unique<IndicatorLedManager>(numberOfPixels, IndicatorLedDriver::NeoPixel(numberOfPixels, pin, type));
}

std::unique_ptr<IndicatorLedManager> IndicatorLedManager::NoOp(uint16_t numberOfPixels) {
    return std::make_unique<IndicatorLedManager>(numberOfPixels, IndicatorLedDriver::NoOp());
}