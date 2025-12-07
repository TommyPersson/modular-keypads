#include "GenericFirmware.h"

GenericFirmware::GenericFirmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger,
    TwoWire& i2c
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger, i2c) {
}

GenericFirmware::~GenericFirmware() = default;

void GenericFirmware::setup() {
    Firmware::setup();

    logger.info("GenericFirmware:started");
}

void GenericFirmware::loop() {
    Firmware::loop();
}
