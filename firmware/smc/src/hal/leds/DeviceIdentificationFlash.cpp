#include "DeviceIdentificationFlash.h"

using namespace smc::hal::leds;

DeviceIdentificationFlash::DeviceIdentificationFlash(IndicatorLeds& leds)
    : leds(leds) {
}

void DeviceIdentificationFlash::setup() {
}

void DeviceIdentificationFlash::update(const uint32_t deltaUs, events::EventQueue& queue) {
    if (!isActive) {
        return;
    }

    elapsedUs += deltaUs;
    if (elapsedUs >= DURATION_US) {
        isActive = false;
        return;
    }

    constexpr uint32_t halfPeriodUs = PERIOD_US / 2;
    // Rounded down to whole steps, so the level (and with it the LEDs) only changes once per step
    const uint32_t phaseUs = elapsedUs % PERIOD_US / STEP_US * STEP_US;
    const uint32_t rampUs = phaseUs < halfPeriodUs ? phaseUs : PERIOD_US - phaseUs;
    const auto level = static_cast<uint8_t>(rampUs * MAX_LEVEL / halfPeriodUs);

    for (uint8_t i = 0; i < leds.size(); i++) {
        leds.setColor(i, level, 0, 0);
    }
}

void DeviceIdentificationFlash::start() {
    elapsedUs = 0;
    isActive = true;
}
