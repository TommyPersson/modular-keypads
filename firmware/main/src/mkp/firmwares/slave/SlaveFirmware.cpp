#include "SlaveFirmware.h"

#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

#include "mkp/components/events/RemoteEventTypes.h"

#include "i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h"
#include "i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"
#include "i2c/commands/RenameDeviceRemoteCommandHandler.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("SlaveFirmware");
}

using namespace mkp::firmwares::base;
using namespace firmwares::slave::i2c::commands;

SlaveFirmware::SlaveFirmware(ServiceLocator& serviceLocator) :
    Firmware(serviceLocator),
    slavePort(serviceLocator.i2cSlavePort) {
}

SlaveFirmware::~SlaveFirmware() = default;

void SlaveFirmware::setup() {
    Firmware::setup();

    auto configuration = serviceLocator.deviceConfigurationManager.getDeviceConfiguration();

    const auto deviceFactory = getDeviceFactory(configuration.type);
    if (deviceFactory == nullptr) {
        logger->error("No device factory found for type: %s", configuration.type.c_str());
        return;
    }

    device = deviceFactory->createLocal(configuration, serviceLocator);
    device->setup();

    registers = &device->getRegisters();

    const auto deviceAddress = configuration.address;
    if (deviceAddress == 0) {
        logger->error("Device needs to have an address configured.");
        return;
    }

    const auto deviceId = configuration.id;
    const auto deviceName = configuration.name;

    mkp::devices::common::i2c::structs::DeviceInformation deviceInformationStruct;
    deviceInformationStruct.deviceId = deviceId;
    deviceInformationStruct.deviceType = configuration.type.front();

    slavePort.updateEndpoint(
        mkp::devices::common::i2c::endpoints::DeviceInformation,
        &deviceInformationStruct
    );

    slavePort.updateEndpoint(
        mkp::devices::common::i2c::endpoints::DeviceName,
        deviceName.c_str()
    );

    slavePort.setup(deviceAddress, device->getI2cPins(), device->getEventInterruptPin());

    slavePort.addCommandHandler(new FlashDeviceIdentificationLightsRemoteCommandHandler(*device));
    slavePort.addCommandHandler(new FlashButtonIdentificationLightRemoteCommandHandler(*device));
    slavePort.addCommandHandler(new RenameDeviceRemoteCommandHandler(*device));

    device->onSwitchEvent().addObserver(this);
    device->onSubDeviceConnectedEvent().addObserver(this);
    device->onSubDeviceDisconnectedEvent().addObserver(this);
}

void SlaveFirmware::loop() {
    Firmware::loop();

    device->loop();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceSwitchEvent& event) {
    logger->info("Device switch event observed?");

    if (event.state == tfw::hal::buttons::ButtonState::PRESSED) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::BUTTON_PRESSED),
                .deviceId = event.deviceId,
                .args = {event.switchNumber}
            }
        );
    } else if (event.state == tfw::hal::buttons::ButtonState::UNPRESSED) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::BUTTON_RELEASED),
                .deviceId = event.deviceId,
                .args = {event.switchNumber}
            }
        );
    }

    slavePort.triggerEventInterrupt();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceConnectedEvent& event) {
    slavePort.enqueueEvent(
        tfw::hal::i2c::Event{
            .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::SUB_DEVICE_CONNECTED),
            .deviceId = event.deviceId,
            .args = {}
        }
    );

    slavePort.triggerEventInterrupt();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceDisconnectedEvent& event) {
    slavePort.enqueueEvent(
        tfw::hal::i2c::Event{
            .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::SUB_DEVICE_DISCONNECTED),
            .deviceId = event.deviceId,
            .args = {}
        }
    );

    slavePort.triggerEventInterrupt();
}
