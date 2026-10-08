#include "DeviceIdentificationFlash.h"

using namespace smc::hal::leds;

DeviceIdentificationFlash::DeviceIdentificationFlash(IndicatorLeds& leds)
    : leds(leds) {
}

void DeviceIdentificationFlash::setup() {
}

void DeviceIdentificationFlash::update(const uint32_t deltaUs, events::EventQueue& queue) {
    if (!animation.update(deltaUs)) {
        return;
    }

    const auto level = animation.level();
    for (uint8_t i = 0; i < leds.size(); i++) {
        leds.setColor(i, level, 0, 0);
    }
}

void DeviceIdentificationFlash::start() {
    animation.start();
}
