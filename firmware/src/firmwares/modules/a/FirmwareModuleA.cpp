#include "FirmwareModuleA.h"

#include "DeviceRuntimeA.h"
#include "LocalRegisterRefresherA.h"
#include "firmwares/common/i2c/EndpointStructs.h"

FirmwareModuleA::FirmwareModuleA(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger,
    TwoWire& i2c
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger, i2c) {

    indicatorLeds = IndicatorLedManager::NeoPixel(12, 6);

    registerRefresher = std::make_unique<LocalRegisterRefresherA>(*registers);
    runtime = std::make_unique<DeviceRuntimeA>(*registers, *indicatorLeds, notifier, logger, DeviceMode::Local);

    i2cSlavePort = std::make_unique<i2c::SlavePort>(i2c, logger);
}

FirmwareModuleA::~FirmwareModuleA() = default;

void FirmwareModuleA::setup() {
    Firmware::setup();

    auto deviceAddress = deviceConfigurationManager.getDeviceAddress();
    if (deviceAddress > 0) {
        const auto deviceId = deviceConfigurationManager.getDeviceId();
        i2c::structs::DeviceInformation deviceInformationStruct;
        std::memcpy(&deviceInformationStruct.deviceId, deviceId.data(), sizeof(deviceInformationStruct.deviceId));
        deviceInformationStruct.deviceType = deviceConfigurationManager.getDeviceType();

        i2cSlavePort->updateEndpoint(i2c::Endpoint::DeviceInformation, &deviceInformationStruct, sizeof(deviceInformationStruct));
        i2cSlavePort->begin(deviceAddress, 1, 0);
    }

    registerRefresher->begin();
    runtime->begin();

    logger.info("FirmwareModuleA:started");
}

void FirmwareModuleA::loop() {
    Firmware::loop();

    registerRefresher->loop();
    runtime->loop();
}
