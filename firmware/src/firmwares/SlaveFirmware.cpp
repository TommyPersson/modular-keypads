#include "SlaveFirmware.h"

// TODO implement

SlaveFirmware::SlaveFirmware(ServiceLocator& serviceLocator) : Firmware(serviceLocator) {
}

SlaveFirmware::~SlaveFirmware() = default;

void SlaveFirmware::setup() {
    Firmware::setup();
}

void SlaveFirmware::loop() {
    Firmware::loop();
}
