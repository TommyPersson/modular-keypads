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

    const auto deviceId = configuration.id;

    mkp::devices::common::i2c::structs::DeviceInformation deviceInformationStruct;
    deviceInformationStruct.deviceId = deviceId;
    deviceInformationStruct.deviceType = configuration.type.front();

    slavePort.updateEndpoint(
        mkp::devices::common::i2c::endpoints::DeviceInformation,
        &deviceInformationStruct
    );

    updateDeviceNameEndpoint();

    // The master assigns the address once it discovers the device
    slavePort.setup(
        deviceId,
        device->getI2cPins(),
        device->getEventInterruptPin(),
        device->getDiscoveryPin()
    );

    slavePort.addCommandHandler(new FlashDeviceIdentificationLightsRemoteCommandHandler(*device));
    slavePort.addCommandHandler(new FlashButtonIdentificationLightRemoteCommandHandler(*device));
    slavePort.addCommandHandler(new RenameDeviceRemoteCommandHandler(*device));

    // Written on the first update, so the endpoints of sub-devices that aren't connected are valid too
    subDeviceInformation.resize(device->getSubDevices().size());
    for (size_t i = 0; i < subDeviceInformation.size(); i++) {
        slavePort.updateEndpoint(
            mkp::devices::common::i2c::endpoints::SubDeviceInformation(i),
            &subDeviceInformation[i]
        );
    }

    device->onSwitchEvent().addObserver(this);
    device->onRotaryEncoderEvent().addObserver(this);
    device->onRenamedEvent().addObserver(this);
    device->onSubDeviceConnectedEvent().addObserver(this);
    device->onSubDeviceDisconnectedEvent().addObserver(this);
    device->onSubDeviceRenamedEvent().addObserver(this);

    // Last, so that the device is fully set up when the master scans it
    slavePort.requestDiscovery();
}

void SlaveFirmware::loop() {
    Firmware::loop();

    slavePort.loop();

    device->loop();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceSwitchEvent& event) {
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
    } else if (event.state == tfw::hal::buttons::ButtonState::LONG_PRESSED) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::BUTTON_LONG_PRESSED),
                .deviceId = event.deviceId,
                .args = {event.switchNumber}
            }
        );
    }

    slavePort.triggerEventInterrupt();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) {
    if (event.direction == tfw::hal::encoders::RotaryEncoderDirection::Clockwise) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::ROTARY_ENCODER_CW),
                .deviceId = event.deviceId,
                .args = {event.encoderNumber}
            }
        );
    } else if (event.direction == tfw::hal::encoders::RotaryEncoderDirection::CounterClockwise) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::ROTARY_ENCODER_CCW),
                .deviceId = event.deviceId,
                .args = {event.encoderNumber}
            }
        );
    }

    slavePort.triggerEventInterrupt();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceConnectedEvent& event) {
    // Updated before the event is sent, as the master reads the endpoints when it receives it
    updateSubDeviceEndpoints();

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
    updateSubDeviceEndpoints();

    slavePort.enqueueEvent(
        tfw::hal::i2c::Event{
            .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::SUB_DEVICE_DISCONNECTED),
            .deviceId = event.deviceId,
            .args = {}
        }
    );

    slavePort.triggerEventInterrupt();
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceRenamedEvent& event) {
    // Updated before the event is sent, as the master reads the endpoints when it receives it
    const auto isDevice = event.deviceId == device->getConfiguration().id;
    if (isDevice) {
        updateDeviceNameEndpoint();
    } else {
        updateSubDeviceEndpoints();
    }

    slavePort.enqueueEvent(
        tfw::hal::i2c::Event{
            .type = static_cast<uint8_t>(
                isDevice
                    ? mkp::components::events::RemoteEventType::DEVICE_RENAMED
                    : mkp::components::events::RemoteEventType::SUB_DEVICE_RENAMED
            ),
            .deviceId = event.deviceId,
            .args = {}
        }
    );

    slavePort.triggerEventInterrupt();
}

void SlaveFirmware::updateDeviceNameEndpoint() {
    mkp::devices::common::i2c::structs::DeviceName deviceName;
    strncpy(deviceName.deviceName, device->getConfiguration().name.c_str(), sizeof(deviceName.deviceName));

    slavePort.updateEndpoint(mkp::devices::common::i2c::endpoints::DeviceName, &deviceName);
}

void SlaveFirmware::updateSubDeviceEndpoints() {
    const auto subDevices = device->getSubDevices();

    for (uint8_t i = 0; i < subDevices.size(); i++) {
        const auto& configuration = subDevices[i]->getConfiguration();

        mkp::devices::common::i2c::structs::SubDeviceInformation information;
        information.deviceId = configuration.id;
        if (configuration.id != 0) {
            strncpy(information.deviceType, configuration.type.c_str(), sizeof(information.deviceType));
            strncpy(information.deviceName, configuration.name.c_str(), sizeof(information.deviceName));
        }

        if (memcmp(&information, &subDeviceInformation[i], sizeof(information)) == 0) {
            continue;
        }

        subDeviceInformation[i] = information;
        slavePort.updateEndpoint(mkp::devices::common::i2c::endpoints::SubDeviceInformation(i), &information);
    }
}
