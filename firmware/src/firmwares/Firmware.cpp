#include "Firmware.h"

#include "common/ServiceLocator.h"
#include "modules/a/FirmwareModuleA.h"
#include "modules/base/commands/ListRegistersCommandHandler.h"
#include "modules/base/commands/PingCommandHandler.h"
#include "modules/base/commands/ReadDeviceAddressCommandHandler.h"
#include "modules/base/commands/ReadDeviceFirmwareVersionCommandHandler.h"
#include "modules/base/commands/ReadDeviceIdCommandHandler.h"
#include "modules/base/commands/ReadDeviceNameCommandHandler.h"
#include "modules/base/commands/ReadDeviceTypeCommandHandler.h"
#include "modules/base/commands/ReadRegisterCommandHandler.h"
#include "modules/base/commands/ResetDeviceCommandHandler.h"
#include "modules/base/commands/SetDeviceAddressCommandHandler.h"
#include "modules/base/commands/SetDeviceNameCommandHandler.h"
#include "modules/base/commands/SetDeviceTypeCommandHandler.h"
#include "modules/generic/GenericFirmware.h"
#include "modules/m/FirmwareModuleM.h"

std::unique_ptr<Firmware> Firmware::create(ServiceLocator& serviceLocator) {

    auto deviceType = serviceLocator.deviceConfigurationManager.getDeviceType();

    switch (deviceType) {
    case 'm':
    case 'M':
        return std::make_unique<FirmwareModuleM>(serviceLocator);
    case 'a':
    case 'A':
        return std::make_unique<FirmwareModuleA>(serviceLocator);
    default:
        return std::make_unique<GenericFirmware>(serviceLocator);
    }
}

Firmware::Firmware(ServiceLocator& serviceLocator) :
    deviceConfigurationManager(serviceLocator.deviceConfigurationManager),
    serialPort(serviceLocator.serialPort),
    notifier(serviceLocator.notifier),
    logger(serviceLocator.logger),
    i2c(serviceLocator.i2c) {

    this->registers = std::make_unique<RegisterManager>();

    this->lineStreamer = std::make_unique<LineStreamer>(serialPort.stream());
    this->commandProcessor = std::make_unique<CommandProcessor>(serialPort.stream(), logger);
    this->lineStreamer->addObserver(this->commandProcessor.get());

    this->addCommandHandler(std::make_shared<PingCommandHandler>(logger));
    this->addCommandHandler(std::make_shared<ReadDeviceIdCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(
        std::make_shared<ReadDeviceFirmwareVersionCommandHandler>(
            deviceConfigurationManager,
            logger
            )
        );
    this->addCommandHandler(std::make_shared<ReadDeviceTypeCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<ReadDeviceAddressCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<ReadDeviceNameCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<SetDeviceAddressCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<SetDeviceTypeCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<SetDeviceNameCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<ResetDeviceCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<ListRegistersCommandHandler>(*registers, logger));
    this->addCommandHandler(std::make_shared<ReadRegisterCommandHandler>(*registers, logger));
}

void Firmware::setup() {
    serialPort.begin(115200);
    deviceConfigurationManager.begin();
    notifier.begin(deviceConfigurationManager.getDeviceId());
}

void Firmware::loop() {
    delayMicroseconds(1000);
    lineStreamer->update();
}

void Firmware::addCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const {
    this->commandProcessor->addHandler(commandHandler);
}

