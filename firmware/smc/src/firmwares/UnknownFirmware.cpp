#include "UnknownFirmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

UnknownFirmware::UnknownFirmware(const uint8_t smcType) : smcType(smcType) {
}

inline void UnknownFirmware::setup() {
    Firmware::setup();

    leds.setup();

    Serial.printf("Unknown module type, falling back to no-op firmware! (smcType = %02x)", smcType);
}
