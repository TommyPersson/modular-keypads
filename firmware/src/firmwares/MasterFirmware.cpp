#include "MasterFirmware.h"

#include "common/DeviceScanner.h"
#include "modules/a/DeviceRuntimeA.h"
#include "modules/m/DeviceRuntimeM.h"

namespace {
    auto logger = common::logging::createLogger("MasterFirmware");
}

MasterFirmware::MasterFirmware(ServiceLocator& serviceLocator) :
    Firmware(serviceLocator) {
}

MasterFirmware::~MasterFirmware() = default;

void MasterFirmware::setup() {
    Firmware::setup();

    auto localDeviceConfiguration = deviceConfigurationManager.getDeviceConfiguration();

    auto localModuleFactory = getModuleFactory(localDeviceConfiguration.type);
    if (localModuleFactory == nullptr) {
        logger->error("No module factory found for type: %c", localDeviceConfiguration.type);
        return;
    }

    auto localDevice = localModuleFactory->createLocal(localDeviceConfiguration, serviceLocator);
    localDevice->setup();

    registers = &localDevice->getRegisters();

    devices.push_back(std::move(localDevice));

    auto pins = localModuleFactory->getI2cPins();
    serviceLocator.i2cClient.setup(pins);

    DeviceScanner scanner(serviceLocator.i2cClient);
    auto scanResult = scanner.scan();

    for (const auto& device : scanResult) {
        logger->info("Found device at %i: %s", device->getConfiguration().address, device->getConfiguration().id.c_str()
            );
        logger->info("Device name: %s", device->getConfiguration().name.c_str());
        logger->info("Device type: %c", device->getConfiguration().type);

        auto moduleFactory = getModuleFactory(device->getConfiguration().type);
        if (moduleFactory == nullptr) {
            logger->error("No module factory found for type: %c", device->getConfiguration().type);
            continue;
        }

        auto remoteDevice = moduleFactory->createRemote(device->getConfiguration(), serviceLocator);
        remoteDevice->setup();
        devices.push_back(std::move(remoteDevice));
    }
}

void MasterFirmware::loop() {
    Firmware::loop();

    for (const auto& device : devices) {
        device->loop();
    }
}
