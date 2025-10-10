#include "GenericFirmware.h"

#include "firmwares/common/commands/CommandProcessor.h"

#include "commands/PingCommandHandler.h"
#include "commands/ReadDeviceAddressCommandHandler.h"
#include "commands/ReadDeviceFirmwareVersionCommandHandler.h"
#include "commands/ReadDeviceIdCommandHandler.h"
#include "commands/ReadDeviceTypeCommandHandler.h"
#include "commands/ResetDeviceCommandHandler.h"
#include "commands/SetDeviceAddressCommandHandler.h"
#include "commands/SetDeviceTypeCommandHandler.h"

GenericFirmware::GenericFirmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, logger) {

    this->registerCommandHandler(std::make_shared<PingCommandHandler>(logger));
    this->registerCommandHandler(std::make_shared<ReadDeviceIdCommandHandler>(deviceConfigurationManager, logger));
    this->registerCommandHandler(
        std::make_shared<ReadDeviceFirmwareVersionCommandHandler>(
            deviceConfigurationManager,
            logger
            )
        );
    this->registerCommandHandler(std::make_shared<ReadDeviceTypeCommandHandler>(deviceConfigurationManager, logger));
    this->registerCommandHandler(std::make_shared<ReadDeviceAddressCommandHandler>(deviceConfigurationManager, logger));
    this->registerCommandHandler(std::make_shared<SetDeviceAddressCommandHandler>(deviceConfigurationManager, logger));
    this->registerCommandHandler(std::make_shared<SetDeviceTypeCommandHandler>(deviceConfigurationManager, logger));
    this->registerCommandHandler(std::make_shared<ResetDeviceCommandHandler>(deviceConfigurationManager, logger));

}

GenericFirmware::~GenericFirmware() = default;

void GenericFirmware::setup() {
    Firmware::setup();

    logger.info("GenericFirmware:started");
}

void GenericFirmware::loop() {
    Firmware::loop();
}
