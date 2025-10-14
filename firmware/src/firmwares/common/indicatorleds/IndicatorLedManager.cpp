#include "IndicatorLedManager.h"

IndicatorLedManager::IndicatorLedManager(
    const uint16_t numberOfPixels,
    const int16_t pin,
    const neoPixelType type
    ) :
    neoPixel(numberOfPixels, pin, type) {

    for (int i = 0; i < numberOfPixels; i++) {
        leds.emplace_back(std::make_shared<IndicatorLed>(neoPixel, i, *this));
    }
}

IndicatorLedManager::~IndicatorLedManager() = default;

void IndicatorLedManager::begin() {
    neoPixel.begin();
    neoPixel.setBrightness(255);
}

std::shared_ptr<SwitchIndicatorLed> IndicatorLedManager::connectToSwitch(
    const uint8_t pixelNumber,
    const SwitchMonitor& switchMonitor
    ) {
    const auto& indicatorLed = get(pixelNumber);
    return switchIndicators.emplace_back(std::make_shared<SwitchIndicatorLed>(switchMonitor, *indicatorLed));
}

void IndicatorLedManager::update() {
    if (isDirty) {
        isDirty = false;
        neoPixel.show();
    }
}

void IndicatorLedManager::markAsDirty() {
    isDirty = true;
}

void IndicatorLedManager::clearDirty() {
    isDirty = false;
}

std::shared_ptr<IndicatorLed>& IndicatorLedManager::get(uint8_t pixelNumber) {
    return leds[pixelNumber];
}

std::unique_ptr<IndicatorLedManager> IndicatorLedManager::NeoPixel(
    uint16_t numberOfPixels,
    int16_t pin,
    neoPixelType type
    ) {
    return std::make_unique<IndicatorLedManager>(numberOfPixels, pin, type);
}
