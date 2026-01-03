#include "SlaveFirmware.h"

#include "utils/i2c/EndpointStructs.h"
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

        utils::i2c::structs::DeviceInformation deviceInformationStruct;
        deviceInformationStruct.deviceId = deviceId;
        deviceInformationStruct.deviceType = configuration.type;

        slavePort.updateEndpoint(
            utils::i2c::Endpoint::DeviceInformation,
            &deviceInformationStruct,
            sizeof(deviceInformationStruct)
            );

        slavePort.updateEndpoint(
            utils::i2c::Endpoint::DeviceName,
            deviceName.c_str(),
            sizeof(utils::i2c::structs::DeviceName)
            );

        slavePort.setup(deviceAddress, pins);
    }
}

void SlaveFirmware::loop() {
    Firmware::loop();

    device->loop();

    utils::i2c::structs::DeviceRegisters registersStruct;
    auto registerData = device->getRegisters().readAll();
    std::memcpy(&registersStruct.data, registerData.data(), sizeof(registersStruct.data));

    slavePort.updateEndpoint(utils::i2c::Endpoint::Registers, &registersStruct, sizeof(registersStruct));
}
