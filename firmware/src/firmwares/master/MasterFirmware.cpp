#include "MasterFirmware.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <esp_system.h>
#include <rom/usb/chip_usb_dw_wrapper.h>
#include <rom/usb/usb_persist.h>
#include <soc/rtc_cntl_reg.h>

#endif

#include <utils/strings.h>
#include <utils/allocations/ArenaUtils.h>

#include "commands/ClearKeyBindingCommandHandler.h"
#include "commands/DeleteMacroCommandHandler.h"
#include "commands/GetTestMode.h"
#include "commands/ListConnectedDevices.h"
#include "commands/ListDeviceCapabilities.h"
#include "commands/ListKeyBindingsCommandHandler.h"
#include "commands/ListStoredMacrosCommandHandler.h"
#include "commands/SaveMacroCommandHandler.h"
#include "commands/SetKeyBindingCommandHandler.h"
#include "commands/SetTestMode.h"

#include "../common/DeviceScanner.h"
#include "../common/keybindings/KeyBindingStorage.h"
#include "../common/macros/MacroStorage.h"

namespace {
    auto logger = common::logging::createLogger("MasterFirmware");
}

MasterFirmware::MasterFirmware(ServiceLocator& serviceLocator)
    : Firmware(serviceLocator) {
    macroStorage = std::make_unique<common::macros::MacroStorage>();
    keyBindingStorage = std::make_unique<common::keybindings::KeyBindingStorage>();
    keyBindingSubSystem = std::make_unique<KeyBindingSubSystem>(
        *macroStorage,
        *keyBindingStorage,
        testModeController,
        serviceLocator.usbConnection
    );

    addCommandHandler(std::make_shared<ListConnectedDevices>(allDevices));
    addCommandHandler(std::make_shared<ListDeviceCapabilities>(allDevices));
    addCommandHandler(std::make_shared<GetTestMode>(testModeController));
    addCommandHandler(std::make_shared<SetTestMode>(testModeController));
    addCommandHandler(std::make_shared<SaveMacroCommandHandler>(*macroStorage));
    addCommandHandler(std::make_shared<DeleteMacroCommandHandler>(*macroStorage));
    addCommandHandler(std::make_shared<ListStoredMacrosCommandHandler>(*macroStorage));
    addCommandHandler(std::make_shared<ListKeyBindingsCommandHandler>(*keyBindingStorage));
    addCommandHandler(std::make_shared<SetKeyBindingCommandHandler>(*keyBindingStorage));
    addCommandHandler(std::make_shared<ClearKeyBindingCommandHandler>(*keyBindingStorage));
}

MasterFirmware::~MasterFirmware() = default;

void MasterFirmware::setup() {
    Firmware::setup();

    macroStorage->setup();
    keyBindingStorage->setup();
    keyBindingSubSystem->setup();

    auto localDeviceConfiguration = deviceConfigurationManager.getDeviceConfiguration();

    auto localModuleFactory = getModuleFactory(localDeviceConfiguration.type);
    if (localModuleFactory == nullptr) {
        logger->error("No module factory found for type: %c", localDeviceConfiguration.type);
        return;
    }

    localDevice = localModuleFactory->createLocal(localDeviceConfiguration, serviceLocator);
    localDevice->setup();
    localDevice->onSwitchEvent().addObserver(this);
    localDevice->onRotaryEncoderEvent().addObserver(this);
    allDevices.push_back(localDevice.get());

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

    keyBindingSubSystem->loop();
}

// TODO allow connected devices to use open drain outputs to signal their presence?
void MasterFirmware::refreshConnectedDevices() {
    allDevices.clear();
    allDevices.push_back(localDevice.get());
    connectedDevices.clear();

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
        connectedDevices.push_back(std::move(remoteDevice));
    }

    for (const auto& device : connectedDevices) {
        allDevices.push_back(device.get());
        device->onSwitchEvent().addObserver(this);
        device->onRotaryEncoderEvent().addObserver(this);
    }
}

void MasterFirmware::observe(const devices::DeviceSwitchEvent& event) {
    keyBindingSubSystem->observe(event);

    // TODO temporary debugging
    if (event.state == SwitchState::PRESSED && event.switchNumber == 5 && event.deviceId == 0x7e2c1a823e6bac0c) {
        refreshConnectedDevices();
    }

#ifdef SOC_USB_OTG_SUPPORTED
    if (event.state == SwitchState::PRESSED && event.switchNumber == 4 && event.deviceId == 0x7e2c1a823e6bac0c) {
        chip_usb_set_persist_flags(USBDC_BOOT_DFU);
        REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
        esp_restart();
    }
#endif
}

void MasterFirmware::observe(const devices::DeviceRotaryEncoderEvent& event) {
    keyBindingSubSystem->observe(event);
}
