#include "MasterFirmware.h"

#include <ranges>
#include <unordered_map>

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

    // How long to wait after the discovery line is pulled low before scanning, so that slaves are ready
    constexpr uint64_t DISCOVERY_SCAN_DELAY_US = 500'000;

    constexpr uint64_t PRESENCE_CHECK_INTERVAL_US = 1'000'000;
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
    observeDevice(*localDevice);
    allDevices.push_back(localDevice.get());

    registers = &localDevice->getRegisters();

    auto pins = localDevice->getI2cPins();
    serviceLocator.i2cClient.setup(pins);

    // Only the master pulls the line up, as all boards are the same and any of them can be the master
    discoveryInputPin = tfw::hal::gpio::InputPin::physical(localDevice->getDiscoveryPin(), INPUT_PULLUP);
    discoveryInputPin->setup();
    discoveryInputPin->setupInterrupt(FALLING);
    discoveryInputPin->onInterruptEvent().addObserver(this);
    discoveryScanCountDown = serviceLocator.clock.createCountDown(DISCOVERY_SCAN_DELAY_US, [this] {
        logger->info("Discovery requested, scanning for devices");
        remoteDeviceScanRequested = true;
    });

    // Lost devices are removed, and notified, by the scan
    presenceCheckTimer = serviceLocator.clock.createTimer(PRESENCE_CHECK_INTERVAL_US, [this](const auto& event) {
        for (const auto& device : remoteDevices) {
            if (!device->checkPresence()) {
                remoteDeviceScanRequested = true;
            }
        }
    });

    slaveEventInterruptInputPin = tfw::hal::gpio::InputPin::physical(localDevice->getEventInterruptPin(), INPUT_PULLUP);
    slaveEventInterruptInputPin->setup();
    slaveEventInterruptInputPin->setupInterrupt(FALLING);
    slaveEventInterruptInputPin->onInterruptEvent().addObserver(this);

    refreshRemoteDevices();
}

void MasterFirmware::loop() {
    Firmware::loop();

    discoveryInputPin->checkForInterrupt();
    discoveryScanCountDown->update();
    presenceCheckTimer->update();

    if (remoteDeviceScanRequested) {
        remoteDeviceScanRequested = false;
        refreshRemoteDevices();
    }

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

// Devices that are found again are kept, so that rescanning doesn't reset their state. Devices that appeared or
// disappeared are notified as connected and disconnected. A device's address and type are assumed never to change.
void MasterFirmware::refreshRemoteDevices() {
    DeviceScanner scanner(serviceLocator.i2cClient);
    const auto scanResult = scanner.scan();

    // All found devices are acknowledged, including known ones that restarted, and ones of unsupported types, as
    // the discovery line stays low until every device has released it
    for (const auto& deviceConfiguration : scanResult) {
        const auto result = serviceLocator.i2cClient.sendCommand(
            deviceConfiguration->address,
            0,
            tfw::hal::i2c::commands::builtin::AcknowledgeDiscovery,
            {}
        );

        if (result.has_error) {
            logger->error("Unable to acknowledge discovery of device at %i", deviceConfiguration->address);
        }
    }

    // Devices are taken out as they're found again, so the remaining ones are the lost devices
    std::unordered_map<uint64_t, std::unique_ptr<RemoteDevice>> previousDevices;
    for (auto& device : remoteDevices) {
        const auto deviceId = device->getConfiguration().id;
        previousDevices.emplace(deviceId, std::move(device));
    }
    remoteDevices.clear();

    std::vector<uint64_t> connectedDeviceIds;

    for (const auto& deviceConfiguration : scanResult) {
        if (auto existing = previousDevices.extract(deviceConfiguration->id)) {
            remoteDevices.push_back(std::move(existing.mapped()));
            continue;
        }

        logger->info("Found device at %i: %08llx", deviceConfiguration->address, deviceConfiguration->id);
        logger->info("Device name: %s", deviceConfiguration->name.c_str());
        logger->info("Device type: %s", deviceConfiguration->type.c_str());

        const auto deviceFactory = getDeviceFactory(deviceConfiguration->type);
        if (deviceFactory == nullptr) {
            logger->error("No device factory found for type: %s", deviceConfiguration->type.c_str());
            continue;
        }

        auto device = deviceFactory->createRemote(*deviceConfiguration, serviceLocator);
        device->setup();
        observeDevice(*device);

        connectedDeviceIds.push_back(deviceConfiguration->id);
        remoteDevices.push_back(std::move(device));
    }

    for (const auto& deviceId : previousDevices | std::views::keys) {
        logger->info("Lost device: %08llx", deviceId);
    }

    rebuildAllDevices();

    // Notified once the devices are in place, as receivers will query them
    for (const auto& deviceId : previousDevices | std::views::keys) {
        notifier->notify(deviceId, {.type = "device.disconnected", .args = {}});
    }

    for (const auto deviceId : connectedDeviceIds) {
        notifier->notify(deviceId, {.type = "device.connected", .args = {}});
    }
}

void MasterFirmware::rebuildAllDevices() {
    allDevices.clear();
    allDevices.push_back(localDevice.get());

    for (const auto& device : remoteDevices) {
        allDevices.push_back(device.get());

        for (const auto& subDevice : device->getSubDevices()) {
            allDevices.push_back(subDevice);
        }
    }

    for (const auto& device : localDevice->getSubDevices()) {
        allDevices.push_back(device);
    }
}

// Called once per device, when it's created. Sub-devices aren't observed, as their events are reported by their
// parent device.
void MasterFirmware::observeDevice(Device& device) {
    device.onSwitchEvent().addObserver(this);
    device.onRotaryEncoderEvent().addObserver(this);
    device.onRenamedEvent().addObserver(this);
    device.onSubDeviceConnectedEvent().addObserver(this);
    device.onSubDeviceDisconnectedEvent().addObserver(this);
    device.onSubDeviceRenamedEvent().addObserver(this);
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
        remoteDeviceScanRequested = true;
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
    } else if (event.pin == discoveryInputPin.get()) {
        discoveryScanCountDown->start();
    }
}
