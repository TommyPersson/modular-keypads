#include "LedFlashAnimation.h"

using namespace smc::hal::leds;

void LedFlashAnimation::start() {
    elapsedUs = 0;
    isActive = true;
}

bool LedFlashAnimation::update(const uint32_t deltaUs) {
    if (!isActive) {
        return false;
    }

    elapsedUs += deltaUs;
    if (elapsedUs >= DURATION_US) {
        isActive = false;
    }

    return isActive;
}

uint8_t LedFlashAnimation::level() const {
    constexpr uint32_t halfPeriodUs = PERIOD_US / 2;
    // Rounded down to whole steps, so the level (and with it the LEDs) only changes once per step
    const uint32_t phaseUs = elapsedUs % PERIOD_US / STEP_US * STEP_US;
    const uint32_t rampUs = phaseUs < halfPeriodUs ? phaseUs : PERIOD_US - phaseUs;
    return static_cast<uint8_t>(rampUs * MAX_LEVEL / halfPeriodUs);
}
