#include "MasterFirmware.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <esp_system.h>
#include <rom/usb/chip_usb_dw_wrapper.h>
#include <rom/usb/usb_persist.h>
#include <soc/rtc_cntl_reg.h>

#endif

#include <firmwares/base/commands/SetDeviceNameCommandHandler.h>

#include "../../mkp/devices/common/DeviceScanner.h"
#include "../common/keybindings/KeyBindingStorage.h"
#include "../common/macros/MacroStorage.h"
#include "commands/ClearKeyBindingCommandHandler.h"
#include "commands/DeleteMacroCommandHandler.h"
#include "commands/FlashButtonIdentificationLightCommandHandler.h"
#include "commands/FlashDeviceIdentificationLightsCommandHandler.h"
#include "commands/GetTestMode.h"
#include "commands/ListConnectedDevices.h"
#include "commands/ListDeviceCapabilities.h"
#include "commands/ListKeyBindingsCommandHandler.h"
#include "commands/ListStoredMacrosCommandHandler.h"
#include "commands/SaveMacroCommandHandler.h"
#include "commands/SetKeyBindingCommandHandler.h"
#include "commands/SetTestMode.h"
#include <tfw/utils/strings.h>
#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("MasterFirmware");
}

using namespace mkp::devices::common;

MasterFirmware::MasterFirmware(ServiceLocator& serviceLocator)
    : Firmware(serviceLocator) {
    macroStorage = std::make_unique<common::macros::MacroStorage>();
    keyBindingStorage = std::make_unique<common::keybindings::KeyBindingStorage>();
    keyBindingSubSystem = std::make_unique<KeyBindingSubSystem>(
        *macroStorage,
        *keyBindingStorage,
        testModeController,
        serviceLocator.usbConnection,
        serviceLocator.metricRegistry
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
    addCommandHandler(std::make_shared<FlashDeviceIdentificationLightsCommandHandler>(allDevices));
    addCommandHandler(std::make_shared<FlashButtonIdentificationLightCommandHandler>(allDevices));
    addCommandHandler(std::make_shared<SetDeviceNameCommandHandler>(allDevices));
    // TODO move all (uart) commands to "master" directory? It is by definition the only one accepting commands.

    loopTimerMetric = serviceLocator.metricRegistry.timer("firmware.master.device_loop_time_us");
}

MasterFirmware::~MasterFirmware() = default;

void MasterFirmware::setup() {
    Firmware::setup();

    macroStorage->setup();
    keyBindingStorage->setup();
    keyBindingSubSystem->setup();

    auto localDeviceConfiguration = deviceConfigurationManager.getDeviceConfiguration();

    auto localModuleFactory = getDeviceFactory(localDeviceConfiguration.type);
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

    auto pins = localDevice->getI2cPins();
    serviceLocator.i2cClient.setup(pins);

    refreshRemoteDevices();
}

void MasterFirmware::loop() {
    Firmware::loop();

    loopTimerMetric->measure(
        [this] {
            localDevice->loop();
            for (const auto& device : remoteDevices) {
                device->loop();
            }
        }
    );

    keyBindingSubSystem->loop();
}

// TODO allow connected devices to use open drain outputs to signal their presence?
void MasterFirmware::refreshRemoteDevices() {
    allDevices.clear();
    allDevices.push_back(localDevice.get());
    remoteDevices.clear();

    DeviceScanner scanner(serviceLocator.i2cClient);
    const auto scanResult = scanner.scan();
    for (const auto& deviceConfiguration : scanResult) {
        logger->info("Found device at %i: %08llx", deviceConfiguration->address, deviceConfiguration->id);
        logger->info("Device name: %s", deviceConfiguration->name.c_str());
        logger->info("Device type: %c", deviceConfiguration->type);

        const auto deviceFactory = getDeviceFactory(deviceConfiguration->type);
        if (deviceFactory == nullptr) {
            logger->error("No device factory found for type: %c", deviceConfiguration->type);
            continue;
        }

        auto device = deviceFactory->createRemote(*deviceConfiguration, serviceLocator);

        remoteDevices.push_back(std::move(device));
    }

    for (const auto& device : remoteDevices) {
        allDevices.push_back(device.get());
        device->setup();
        device->onSwitchEvent().addObserver(this);
        device->onRotaryEncoderEvent().addObserver(this);
    }
}

void MasterFirmware::observe(const mkp::devices::common::DeviceSwitchEvent& event) {
    keyBindingSubSystem->observe(event);

    // TODO temporary debugging
    if (event.state == tfw::hal::buttons::ButtonState::PRESSED && event.switchNumber == 11) {
        refreshRemoteDevices();
    }

#ifdef SOC_USB_OTG_SUPPORTED
    if (event.state == tfw::hal::buttons::ButtonState::PRESSED && event.switchNumber == 12) {
        chip_usb_set_persist_flags(USBDC_BOOT_DFU);
        REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
        esp_restart();
    }
#endif
}

void MasterFirmware::observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) {
    keyBindingSubSystem->observe(event);
}
