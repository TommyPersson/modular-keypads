#include "Smc1Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

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
    }
}