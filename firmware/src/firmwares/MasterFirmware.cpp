#include "MasterFirmware.h"

// TODO implement

MasterFirmware::MasterFirmware(ServiceLocator& serviceLocator) : Firmware(serviceLocator) {
}

MasterFirmware::~MasterFirmware() = default;

void MasterFirmware::setup() {
    Firmware::setup();
}

void MasterFirmware::loop() {
    Firmware::loop();
}
