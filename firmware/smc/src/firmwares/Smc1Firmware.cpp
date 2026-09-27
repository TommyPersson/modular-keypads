#include "Smc1Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

namespace {
    constexpr uint8_t LED_BRIGHTNESS = 50;
}

Smc1Firmware::Smc1Firmware()
    : Firmware(4) {
}

Smc1Firmware::~Smc1Firmware() = default;

void Smc1Firmware::doSetup() {
    for (auto& button : buttons) {
        button.setup();
    }

    Serial.printf("SMC1 firmware setup complete!\r\n");
}

void Smc1Firmware::doUpdate(const uint32_t deltaUs, smc::events::EventQueue& queue) {
    for (auto& button : buttons) {
        button.update(deltaUs, queue);

        auto ledIndex = buttonLedMap[button.number - 1];
        if (button.isPressed()) {
            leds.setColor(ledIndex, 255, 255, 255, LED_BRIGHTNESS);
        } else {
            leds.setColor(ledIndex, 0, 0, 0, LED_BRIGHTNESS);
        }
    }
}