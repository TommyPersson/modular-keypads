#include "GenericFirmware.h"

GenericFirmware::GenericFirmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, logger) {
}

GenericFirmware::~GenericFirmware() = default;

void GenericFirmware::setup() {
    Firmware::setup();

    logger.info("GenericFirmware:started");
}

void GenericFirmware::loop() {
    Firmware::loop();
}
