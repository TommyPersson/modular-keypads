#include "Firmware.h"

#include "common/ServiceLocator.h"
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
#include "firmwares/MasterFirmware.h"
#include "firmwares/SlaveFirmware.h"
#include "modules/m/DeviceModuleFactoryM.h"
#include "modules/a/DeviceModuleFactoryA.h"

Firmware::Firmware(ServiceLocator& serviceLocator) :
    deviceConfigurationManager(serviceLocator.deviceConfigurationManager),
    serialPort(serviceLocator.serialPort),
    notifier(serviceLocator.notifier),
    i2c(serviceLocator.i2c),
    serviceLocator(serviceLocator) {

    this->lineStreamer = std::make_unique<LineStreamer>(serialPort.stream());
    this->commandProcessor = std::make_unique<CommandProcessor>(serialPort.stream());
    this->lineStreamer->addObserver(this->commandProcessor.get());

    this->addCommandHandler(std::make_shared<PingCommandHandler>());
    this->addCommandHandler(std::make_shared<ReadDeviceIdCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<ReadDeviceFirmwareVersionCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<ReadDeviceTypeCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<ReadDeviceAddressCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<ReadDeviceNameCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<SetDeviceAddressCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<SetDeviceTypeCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<SetDeviceNameCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<ResetDeviceCommandHandler>(deviceConfigurationManager));
    this->addCommandHandler(std::make_shared<ListRegistersCommandHandler>(registers));
    this->addCommandHandler(std::make_shared<ReadRegisterCommandHandler>(registers));

    moduleFactories.emplace_back(std::make_unique<devices::m::DeviceModuleFactoryM>());
    moduleFactories.emplace_back(std::make_unique<devices::a::DeviceModuleFactoryA>());
}

devices::common::DeviceModuleFactory* Firmware::getModuleFactory(char deviceType) const {
    for (const auto& moduleFactory : moduleFactories) {
        if (moduleFactory->matches(deviceType)) {
            return moduleFactory.get();
        }
    }

    return nullptr;
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

std::unique_ptr<Firmware> Firmware::create(ServiceLocator& serviceLocator) {
    const auto deviceAddress = serviceLocator.deviceConfigurationManager.getDeviceAddress();
    if (deviceAddress == 10) {
        return std::make_unique<MasterFirmware>(serviceLocator);
    }

    return std::make_unique<SlaveFirmware>(serviceLocator);
}