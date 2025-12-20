#include "MasterFirmware.h"

#include "common/DeviceScanner.h"

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
        logger->info("Found device at %i: %08llx", device->getConfiguration().address, device->getConfiguration().id);
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

    for (const auto& device : devices) {
        device->onSwitchEvent().addObserver(this);
    }
}

void MasterFirmware::loop() {
    Firmware::loop();

    for (const auto& device : devices) {
        device->loop();
    }
}

struct KeyBinding {
    uint64_t deviceId;
    uint8_t switchNumber;
};

namespace {

}

void MasterFirmware::observe(const devices::DeviceSwitchEvent& event) {
    if (event.state == SwitchState::PRESSED) {
        serviceLocator.usbConnection.sendAction(*usb::KeyPressAction::keyPress(0x24));
    }

    logger->info("Got key event: %08llx, %i, %i", event.deviceId, event.switchNumber, event.state);
}
