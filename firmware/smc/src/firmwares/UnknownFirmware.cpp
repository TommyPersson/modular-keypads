#include "UnknownFirmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

UnknownFirmware::UnknownFirmware(const uint8_t smcType)
    : Firmware(0),
      smcType(smcType) {
}

void UnknownFirmware::doSetup() {
    Serial.printf("Unknown module type, falling back to no-op firmware! (smcType = %02x)", smcType);
}
