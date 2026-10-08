#include "ButtonIdentificationFlash.h"

using namespace smc::hal::leds;

ButtonIdentificationFlash::ButtonIdentificationFlash(IndicatorLeds& leds)
    : leds(leds) {
}

void ButtonIdentificationFlash::setup() {
}

void ButtonIdentificationFlash::update(const uint32_t deltaUs, events::EventQueue& queue) {
    if (!animation.update(deltaUs)) {
        return;
    }

    leds.setColor(ledIndex, animation.level(), 0, 0);
}

void ButtonIdentificationFlash::start(const uint8_t ledIndex) {
    this->ledIndex = ledIndex;
    animation.start();
}
