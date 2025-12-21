#include "MasterFirmware.h"

#include "common/DeviceScanner.h"

namespace {
    auto logger = common::logging::createLogger("MasterFirmware");
}

namespace {
    struct KeyBinding {
        uint64_t deviceId;
        uint8_t switchNumber;
        std::shared_ptr<usb::Action> action;
    };

    std::vector<KeyBinding> keyBindings;
}

MasterFirmware::MasterFirmware(ServiceLocator& serviceLocator)
    : Firmware(serviceLocator) {
    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 1,
            .action = usb::Action::keyPress({0xe1, 0x04}),
            // a
        }
    );

    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 2,
            .action = usb::Action::keyPress({0xe1, 0x05}),
            // b
        }
    );

    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 3,
            .action = usb::Action::keyPress({0xe1, 0x06}),
            // c
        }
    );

    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 4,
            .action = usb::Action::keyPress({0xe1, 0x07}),
            // d
        }
        );

    keyBindings.push_back(
        {
            .deviceId = 0x9ab574502dcdf51d,
            .switchNumber = 2,
            .action = usb::Action::keyPress({0xe2, 0x40}), // Alt+F7
        }
        );

    keyBindings.push_back(
        {
            .deviceId = 0x9ab574502dcdf51d,
            .switchNumber = 10,
            .action = usb::Action::keyPress({0xe1, 0x3f}), // Shift+F6
        }
    );
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


void MasterFirmware::observe(const devices::DeviceSwitchEvent& event) {
    if (event.state == SwitchState::PRESSED) {
        for (auto& binding : keyBindings) {
            if (binding.deviceId == event.deviceId && binding.switchNumber == event.switchNumber) {
                serviceLocator.usbConnection.sendAction(*binding.action);
            }
        }
    }
}
