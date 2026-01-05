#include "SlaveFirmware.h"

#include "utils/strings.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("SlaveFirmware");
}

SlaveFirmware::SlaveFirmware(ServiceLocator& serviceLocator) :
    slavePort(serviceLocator.i2cSlavePort), Firmware(serviceLocator) {
}

SlaveFirmware::~SlaveFirmware() = default;

void SlaveFirmware::setup() {
    Firmware::setup();

    auto configuration = serviceLocator.deviceConfigurationManager.getDeviceConfiguration();

    auto localModuleFactory = getModuleFactory(configuration.type);
    if (localModuleFactory == nullptr) {
        logger->error("No module factory found for type: %c", configuration.type);
        return;
    }

    device = localModuleFactory->createLocal(configuration, serviceLocator);
    device->setup();

    registers = &device->getRegisters();

    auto pins = localModuleFactory->getI2cPins();

    auto deviceAddress = configuration.address;
    if (deviceAddress > 0) {
        const auto deviceId = configuration.id;
        const auto deviceName = configuration.name;

        devices::common::i2c::structs::DeviceInformation deviceInformationStruct;
        deviceInformationStruct.deviceId = deviceId;
        deviceInformationStruct.deviceType = configuration.type;

        slavePort.updateEndpoint(
            devices::common::i2c::endpoints::DeviceInformation,
            &deviceInformationStruct
        );

        slavePort.updateEndpoint(
            devices::common::i2c::endpoints::DeviceName,
            deviceName.c_str()
        );

        slavePort.setup(deviceAddress, pins);
    }
}

void SlaveFirmware::loop() {
    Firmware::loop();

    device->loop();

    devices::common::i2c::structs::DeviceRegisters registersStruct;
    auto registerData = device->getRegisters().readAll();
    std::memcpy(&registersStruct.data, registerData.data(), sizeof(registersStruct.data));

    slavePort.updateEndpoint(
        devices::common::i2c::endpoints::DeviceRegisters,
        &registersStruct,
        device->getRegisterDescriptors().size()
    );
}
