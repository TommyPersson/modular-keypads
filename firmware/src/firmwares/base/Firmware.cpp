#include "Firmware.h"

#include "../common/ServiceLocator.h"
#include "commands/ListRegistersCommandHandler.h"
#include "commands/PingCommandHandler.h"
#include "commands/ReadDeviceAddressCommandHandler.h"
#include "commands/ReadDeviceFirmwareVersionCommandHandler.h"
#include "commands/ReadDeviceIdCommandHandler.h"
#include "commands/ReadDeviceNameCommandHandler.h"
#include "commands/ReadDeviceTypeCommandHandler.h"
#include "commands/ReadRegisterCommandHandler.h"
#include "commands/ReadMetricsCommandHandler.h"
#include "commands/ResetDeviceCommandHandler.h"
#include "commands/SetDeviceAddressCommandHandler.h"
#include "commands/SetDeviceNameCommandHandler.h"
#include "commands/SetDeviceTypeCommandHandler.h"
#include "../master/MasterFirmware.h"
#include "../slave/SlaveFirmware.h"
#include "../modules/m/DeviceModuleFactoryM.h"
#include "../modules/a/DeviceModuleFactoryA.h"

Firmware::Firmware(ServiceLocator& serviceLocator) :
    deviceConfigurationManager(serviceLocator.deviceConfigurationManager),
    serialPort(serviceLocator.serialPort),
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
    this->addCommandHandler(std::make_shared<ReadMetricsCommandHandler>());

    moduleFactories.emplace_back(std::make_unique<devices::m::DeviceModuleFactoryM>());
    moduleFactories.emplace_back(std::make_unique<devices::a::DeviceModuleFactoryA>());
}

devices::DeviceModuleFactory* Firmware::getModuleFactory(char deviceType) const {
    for (const auto& moduleFactory : moduleFactories) {
        if (moduleFactory->matches(deviceType)) {
            return moduleFactory.get();
        }
    }

    return nullptr;
}

void Firmware::setup() {
    serialPort.begin(115200);
    serviceLocator.usbConnection.setup();
    deviceConfigurationManager.begin();
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