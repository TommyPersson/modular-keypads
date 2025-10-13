#include "Firmware.h"

#include "modules/a/FirmwareModuleA.h"
#include "modules/base/commands/PingCommandHandler.h"
#include "modules/base/commands/ReadDeviceAddressCommandHandler.h"
#include "modules/base/commands/ReadDeviceFirmwareVersionCommandHandler.h"
#include "modules/base/commands/ReadDeviceIdCommandHandler.h"
#include "modules/base/commands/ReadDeviceTypeCommandHandler.h"
#include "modules/base/commands/ResetDeviceCommandHandler.h"
#include "modules/base/commands/SetDeviceAddressCommandHandler.h"
#include "modules/base/commands/SetDeviceTypeCommandHandler.h"
#include "modules/generic/GenericFirmware.h"

std::unique_ptr<Firmware> Firmware::create(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger
    ) {

    auto deviceType = deviceConfigurationManager.getDeviceType();

    switch (deviceType) {
    case 'a':
    case 'A':
        return std::make_unique<FirmwareModuleA>(deviceConfigurationManager, serialPort, notifier, logger);
    default:
        return std::make_unique<GenericFirmware>(deviceConfigurationManager, serialPort, notifier, logger);
    }
}

Firmware::Firmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger
    ) :
    deviceConfigurationManager(deviceConfigurationManager),
    serialPort(serialPort),
    notifier(notifier),
    logger(logger) {

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
    this->addCommandHandler(std::make_shared<SetDeviceAddressCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<SetDeviceTypeCommandHandler>(deviceConfigurationManager, logger));
    this->addCommandHandler(std::make_shared<ResetDeviceCommandHandler>(deviceConfigurationManager, logger));
}

void Firmware::setup() {
    serialPort.begin(115200);
    deviceConfigurationManager.begin();
}

void Firmware::loop() {
    delayMicroseconds(1000);
    lineStreamer->update();
}

void Firmware::addCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const {
    this->commandProcessor->addHandler(commandHandler);;
}

std::shared_ptr<Register> Firmware::addRegister(const std::string& name) const {
    return this->registers->add(name);

}
