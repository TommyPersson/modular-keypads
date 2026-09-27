#include "Smc2Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

Smc2Firmware::Smc2Firmware()
    : Firmware(6) {
}

Smc2Firmware::~Smc2Firmware() = default;

void Smc2Firmware::doSetup() {
    for (auto& button : buttons) {
        button.setup();
    }

    Serial.printf("SMC2 firmware setup complete!\r\n");
}

void Smc2Firmware::doUpdate(const uint32_t deltaUs, smc::events::EventQueue& queue) {
    for (auto& button : buttons) {
        button.update(deltaUs, queue);
    }
}
