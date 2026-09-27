#include "Smc2Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

namespace {
    constexpr uint8_t LED_BRIGHTNESS = 50;
}

Smc2Firmware::Smc2Firmware()
    : Firmware(6) {
}

Smc2Firmware::~Smc2Firmware() = default;

void Smc2Firmware::doSetup() {
    leds.setColor(5, 255, 0, 0, LED_BRIGHTNESS);
    leds.setColor(4, 0, 255, 0, LED_BRIGHTNESS);
    leds.setColor(3, 0, 0, 255, LED_BRIGHTNESS);
    leds.setColor(2, 255, 255, 0, LED_BRIGHTNESS);
    leds.setColor(1, 0, 255, 255, LED_BRIGHTNESS);
    leds.setColor(0, 255, 0, 255, LED_BRIGHTNESS);

    for (auto& button : buttons) {
        button.setup();
    }

    Serial.printf("SMC2 firmware setup complete!\r\n");
}

void Smc2Firmware::doUpdate(const uint32_t deltaUs, smc::events::EventQueue& queue) {
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
