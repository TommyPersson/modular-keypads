#include "Smc2Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

Smc2Firmware::Smc2Firmware() {
}

Smc2Firmware::~Smc2Firmware() {
}

void Smc2Firmware::setup() {
    Firmware::setup();

    leds.setup();
    leds.setBrightness(50);
    leds.setColor(5, 255, 0, 0);
    leds.setColor(4, 0, 255, 0);
    leds.setColor(3, 0, 0, 255);
    leds.setColor(2, 255, 255, 0);
    leds.setColor(1, 0, 255, 255);
    leds.setColor(0, 255, 0, 255);

    for (auto& button : buttons) {
        button.setup();
    }

    Serial.printf("SMC2 firmware setup complete!\r\n");
}

void Smc2Firmware::update(const uint32_t deltaUs, smc::events::EventQueue& queue) {
    Firmware::update(deltaUs, queue);

    for (auto& button : buttons) {
        button.update(deltaUs, queue);

        auto ledIndex = buttonLedMap[button.number - 1];
        if (button.isPressed()) {
            leds.setColor(ledIndex, 255, 255, 255);
        } else {
            leds.setColor(ledIndex, 0, 0, 0);
        }
    }

    leds.update(deltaUs, queue);
}
