#include "GenericFirmware.h"

GenericFirmware::GenericFirmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger) {
}

GenericFirmware::~GenericFirmware() = default;

void GenericFirmware::setup() {
    Firmware::setup();

    logger.info("GenericFirmware:started");
}

void GenericFirmware::loop() {
    Firmware::loop();
}
