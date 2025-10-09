#include "GenericFirmware.h"

#include "firmwares/common/commands/CommandProcessor.h"

#include "commands/PingCommandHandler.h"
#include "commands/ReadDeviceIdCommandHandler.h"
#include "commands/ReadDeviceFirmwareVersionCommandHandler.h"
#include "commands/ReadDeviceTypeCommandHandler.h"
#include "commands/ReadDeviceAddressCommandHandler.h"

GenericFirmware::GenericFirmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, logger) {

    this->registerCommandHandler(std::make_shared<PingCommandHandler>());
    this->registerCommandHandler(std::make_shared<ReadDeviceIdCommandHandler>(deviceConfigurationManager));
    this->registerCommandHandler(std::make_shared<ReadDeviceFirmwareVersionCommandHandler>(deviceConfigurationManager));
    this->registerCommandHandler(std::make_shared<ReadDeviceTypeCommandHandler>(deviceConfigurationManager));
    this->registerCommandHandler(std::make_shared<ReadDeviceAddressCommandHandler>(deviceConfigurationManager));

}

GenericFirmware::~GenericFirmware() = default;

void GenericFirmware::setup() {
    Firmware::setup();

    logger.info("GenericFirmware:started");
}

void GenericFirmware::loop() {
    Firmware::loop();
}
