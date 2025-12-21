#include "MasterFirmware.h"

#include "../common/DeviceScanner.h"

#include "esp_system.h"
#include "soc/rtc_cntl_reg.h"
#include "rom/usb/chip_usb_dw_wrapper.h"
#include "rom/usb/usb_persist.h"

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
            .action = usb::Action::keyPress({0xe1, 0x04}), // a
        }
    );

    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 2,
            .action = usb::Action::keyPress({0xe1, 0x05}), // b
        }
    );

    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 3,
            .action = usb::Action::keyPress({0xe1, 0x06}), // c
        }
    );

    keyBindings.push_back(
        {
            .deviceId = 0x7e2c1a823e6bac0c,
            .switchNumber = 4,
            .action = usb::Action::keyPress({0xe1, 0x07}), // d
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

    localDevice = localModuleFactory->createLocal(localDeviceConfiguration, serviceLocator);
    localDevice->setup();
    localDevice->onSwitchEvent().addObserver(this);

    registers = &localDevice->getRegisters();

    auto pins = localModuleFactory->getI2cPins();
    serviceLocator.i2cClient.setup(pins);

    refreshConnectedDevices();
}

void MasterFirmware::loop() {
    Firmware::loop();

    localDevice->loop();
    for (const auto& device : connectedDevices) {
        device->loop();
    }
}

// TODO allow connected devices to use open drain outputs to signal their presence?
void MasterFirmware::refreshConnectedDevices() {
    DeviceScanner scanner(serviceLocator.i2cClient);
    auto scanResult = scanner.scan();

    connectedDevices.clear();
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
        connectedDevices.push_back(std::move(remoteDevice));
    }

    for (const auto& device : connectedDevices) {
        device->onSwitchEvent().addObserver(this);
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

    // TODO temporary debugging
    if (event.state == SwitchState::PRESSED && event.switchNumber == 5 && event.deviceId == 0x7e2c1a823e6bac0c) {
        refreshConnectedDevices();
    }

    if (event.state == SwitchState::PRESSED && event.switchNumber == 4 && event.deviceId == 0x7e2c1a823e6bac0c) {
        chip_usb_set_persist_flags(USBDC_BOOT_DFU);
        REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
        esp_restart();
    }
}
