#include "GenericFirmware.h"

GenericFirmware::GenericFirmware(ServiceLocator& serviceLocator) :
    Firmware(serviceLocator) {
}

GenericFirmware::~GenericFirmware() = default;

void GenericFirmware::setup() {
    Firmware::setup();

    logger.info("GenericFirmware:started");
}

void GenericFirmware::loop() {
    Firmware::loop();
}
