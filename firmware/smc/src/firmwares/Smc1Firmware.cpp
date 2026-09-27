#include "Smc1Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

Smc1Firmware::Smc1Firmware() {

}

Smc1Firmware::~Smc1Firmware() {
}

void Smc1Firmware::setup() {
    Firmware::setup();

    leds.setup();
    leds.setBrightness(50);

    for (auto& button : buttons) {
        button.setup();
    }

    Serial.printf("SMC1 firmware setup complete!\r\n");
}

void Smc1Firmware::update(const uint32_t deltaUs, smc::events::EventQueue& queue) {
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