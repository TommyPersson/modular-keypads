#include "MasterFirmware.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <esp_system.h>
#include <rom/usb/chip_usb_dw_wrapper.h>
#include <rom/usb/usb_persist.h>
#include <soc/rtc_cntl_reg.h>

#endif

#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

#include "commands/All.h"
#include "mkp/devices/common/DeviceScanner.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("MasterFirmware");
}

using namespace mkp::components::keybindings;
using namespace mkp::components::macros;
using namespace mkp::devices::common;
using namespace mkp::firmwares::base;
using namespace mkp::firmwares::master::commands;

MasterFirmware::MasterFirmware(ServiceLocator& serviceLocator)
    : Firmware(serviceLocator) {
    macroStorage = std::make_unique<MacroStorage>(serviceLocator.fileSystem);
    keyBindingStorage = std::make_unique<KeyBindingStorage>(serviceLocator.fileSystem);
    keyBindingExecutor = std::make_unique<KeyBindingExecutor>(
        *macroStorage,
        *keyBindingStorage,
        testModeController,
        serviceLocator.usbConnection,
        serviceLocator.metricRegistry
    );

    notifier = serviceLocator.notifierFactory.create();

    addCommandHandler(std::make_shared<Ping>());
    addCommandHandler(std::make_shared<ReadDeviceId>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<ReadDeviceFirmwareVersion>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<ReadDeviceType>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<ReadDeviceAddress>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<ReadDeviceName>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<SetDeviceAddress>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<ResetDevice>(deviceConfigurationManager));
    addCommandHandler(std::make_shared<ListRegisters>(registers));
    addCommandHandler(std::make_shared<ReadRegister>(registers));
    addCommandHandler(std::make_shared<ListRegisterValues>(registers));
    addCommandHandler(std::make_shared<ReadMetrics>(serviceLocator.metricRegistry));
    addCommandHandler(std::make_shared<ListConnectedDevices>(allDevices));
    addCommandHandler(std::make_shared<ListDeviceCapabilities>(allDevices));
    addCommandHandler(std::make_shared<GetTestMode>(testModeController));
    addCommandHandler(std::make_shared<SetTestMode>(testModeController));
    addCommandHandler(std::make_shared<SaveMacro>(*macroStorage));
    addCommandHandler(std::make_shared<DeleteMacro>(*macroStorage));
    addCommandHandler(std::make_shared<ListStoredMacros>(*macroStorage));
    addCommandHandler(std::make_shared<ListKeyBindings>(*keyBindingStorage));
    addCommandHandler(std::make_shared<SetKeyBinding>(*keyBindingStorage));
    addCommandHandler(std::make_shared<ClearKeyBinding>(*keyBindingStorage));
    addCommandHandler(std::make_shared<FlashDeviceIdentificationLights>(allDevices));
    addCommandHandler(std::make_shared<FlashButtonIdentificationLight>(allDevices));
    addCommandHandler(std::make_shared<SetDeviceName>(allDevices));

    loopTimerMetric = serviceLocator.metricRegistry.timer("firmware.master.device_loop_time_us");
}

MasterFirmware::~MasterFirmware() = default;

void MasterFirmware::setup() {
    Firmware::setup();

    macroStorage->setup();
    keyBindingStorage->setup();
    keyBindingExecutor->setup();

    auto localDeviceConfiguration = deviceConfigurationManager.getDeviceConfiguration();

    auto localModuleFactory = getDeviceFactory(localDeviceConfiguration.type);
    if (localModuleFactory == nullptr) {
        logger->error("No module factory found for type: %s", localDeviceConfiguration.type.c_str());
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

    slaveEventInterruptInputPin = tfw::hal::gpio::InputPin::physical(localDevice->getEventInterruptPin(), INPUT_PULLUP);
    slaveEventInterruptInputPin->setup();
    slaveEventInterruptInputPin->setupInterrupt(RISING);
    slaveEventInterruptInputPin->onInterruptEvent().addObserver(this);
}

void MasterFirmware::loop() {
    Firmware::loop();

    slaveEventInterruptInputPin->checkForInterrupt();

    loopTimerMetric->measure(
        [this] {
            localDevice->loop();
            for (const auto& device : remoteDevices) {
                device->loop();
            }
        }
    );

    keyBindingExecutor->loop();
}

void MasterFirmware::observe(const DeviceConnectedEvent& event) {
    notifier->notify(event.deviceId, {.type = "device.connected", .args = {}});
}

void MasterFirmware::observe(const DeviceDisconnectedEvent& event) {
    notifier->notify(event.deviceId, {.type = "device.disconnected", .args = {}});
}

void MasterFirmware::observe(const DeviceRenamedEvent& event) {
    notifier->notify(event.deviceId, {.type = "device.renamed", .args = {}});
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
        logger->info("Device type: %s", deviceConfiguration->type.c_str());

        const auto deviceFactory = getDeviceFactory(deviceConfiguration->type);
        if (deviceFactory == nullptr) {
            logger->error("No device factory found for type: %s", deviceConfiguration->type.c_str());
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

        // Their events are read and reported by the parent remote device
        for (const auto& subDevice : device->getSubDevices()) {
            allDevices.push_back(subDevice);
        }
    }

    for (const auto& device : localDevice->getSubDevices()) {
        allDevices.push_back(device);
    }

    for (const auto& device : allDevices) {
        device->onRenamedEvent().addObserver(this);
        device->onSubDeviceConnectedEvent().addObserver(this);
        device->onSubDeviceDisconnectedEvent().addObserver(this);
        device->onSubDeviceRenamedEvent().addObserver(this);
    }
}

void MasterFirmware::observe(const DeviceSwitchEvent& event) {
    if (event.state == tfw::hal::buttons::ButtonState::PRESSED) {
        notifier->notify(event.deviceId, {.type = "switch.pressed", .args = {std::to_string(event.switchNumber)}});
    } else if (event.state == tfw::hal::buttons::ButtonState::UNPRESSED) {
        notifier->notify(event.deviceId, {.type = "switch.released", .args = {std::to_string(event.switchNumber)}});
    }

    keyBindingExecutor->observe(event);

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

void MasterFirmware::observe(const DeviceRotaryEncoderEvent& event) {
    if (event.direction == tfw::hal::encoders::RotaryEncoderDirection::Clockwise) {
        notifier->notify(
            event.deviceId,
            {.type = "encoder.rotated.clockwise", .args = {std::to_string(event.encoderNumber)}}
        );
    } else if (event.direction == tfw::hal::encoders::RotaryEncoderDirection::CounterClockwise) {
        notifier->notify(
            event.deviceId,
            {.type = "encoder.rotated.counter-clockwise", .args = {std::to_string(event.encoderNumber)}}
        );
    }

    keyBindingExecutor->observe(event);
}

void MasterFirmware::observe(const tfw::hal::gpio::InputPinInterruptEvent& event) {
    if (event.pin == slaveEventInterruptInputPin.get()) {
        for (const auto& device : remoteDevices) {
            device->onRemoteEventsAvailable();
        }
    }
}
