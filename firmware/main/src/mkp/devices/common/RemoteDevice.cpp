#include "RemoteDevice.h"

#include <tfw/hal/logging/Logger.h>

#include "mkp/components/events/RemoteEventTypes.h"
#include "mkp/firmwares/slave/i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h"
#include "mkp/firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"
#include "mkp/firmwares/slave/i2c/commands/RenameDeviceRemoteCommandHandler.h"

using namespace mkp::devices::common;
using namespace mkp::components::events;

namespace {
    auto logger = tfw::hal::logging::createLogger("RemoteDevice");

    // Tolerates the occasional failed transfer, without being slow to notice lost devices
    constexpr uint8_t MAX_MISSED_PRESENCE_CHECKS = 3;
}


RemoteDevice::RemoteDevice(
    const DeviceConfiguration& configuration,
    const std::vector<std::shared_ptr<DeviceCapability>>& capabilities,
    tfw::hal::i2c::Client& i2cClient,
    const uint8_t numSubDevices
) : configuration(configuration),
    capabilities(capabilities),
    i2cClient(i2cClient) {
    for (uint8_t i = 0; i < numSubDevices; i++) {
        // Slots start at 1, like the numbers of local sub-devices
        subDevices.push_back(std::make_unique<RemoteSubDevice>(configuration, i + 1, i2cClient));
        subDevicePointers.push_back(subDevices.back().get());
    }
}

RemoteDevice::~RemoteDevice() = default;

void RemoteDevice::setup() {
    refreshSubDevices();
}

void RemoteDevice::loop() {
    if (!remoteEventsAvailable) {
        return;
    }

    remoteEventsAvailable = false;

    while (true) {
        const auto result = i2cClient.readEndpoint(configuration.address, tfw::hal::i2c::endpoints::builtin::Events);
        if (result.has_error) {
            return;
        }

        const auto remoteEvent = *result.value;
        if (remoteEvent.type == 0) {
            return;
        }

        const auto remoteEventType = static_cast<RemoteEventType>(remoteEvent.type);

        if (remoteEventType == RemoteEventType::BUTTON_PRESSED) {
            this->deviceSwitchEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .switchNumber = remoteEvent.args[0],
                    .state = tfw::hal::buttons::ButtonState::PRESSED,
                }
            );
        } else if (remoteEventType == RemoteEventType::BUTTON_RELEASED) {
            this->deviceSwitchEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .switchNumber = remoteEvent.args[0],
                    .state = tfw::hal::buttons::ButtonState::UNPRESSED,
                }
            );
        } else if (remoteEventType == RemoteEventType::BUTTON_LONG_PRESSED) {
            this->deviceSwitchEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .switchNumber = remoteEvent.args[0],
                    .state = tfw::hal::buttons::ButtonState::LONG_PRESSED,
                }
            );
        } else if (remoteEventType == RemoteEventType::ROTARY_ENCODER_CW) {
            this->deviceRotaryEncoderEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .encoderNumber = remoteEvent.args[0],
                    .direction = tfw::hal::encoders::RotaryEncoderDirection::Clockwise,
                }
            );
        } else if (remoteEventType == RemoteEventType::ROTARY_ENCODER_CCW) {
            this->deviceRotaryEncoderEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .encoderNumber = remoteEvent.args[0],
                    .direction = tfw::hal::encoders::RotaryEncoderDirection::CounterClockwise,
                }
            );
        } else if (remoteEventType == RemoteEventType::SUB_DEVICE_CONNECTED) {
            refreshSubDevices();
            this->subDeviceConnectedSubject.notify({.deviceId = remoteEvent.deviceId});
        } else if (remoteEventType == RemoteEventType::SUB_DEVICE_DISCONNECTED) {
            refreshSubDevices();
            this->subDeviceDisconnectedSubject.notify({.deviceId = remoteEvent.deviceId});
        } else if (remoteEventType == RemoteEventType::DEVICE_RENAMED) {
            refreshName();
            this->renamedSubject.notify({.deviceId = configuration.id});
        } else if (remoteEventType == RemoteEventType::SUB_DEVICE_RENAMED) {
            refreshSubDevices();
            this->subDeviceRenamedSubject.notify({.deviceId = remoteEvent.deviceId});
        }
    }
}

const DeviceConfiguration& RemoteDevice::getConfiguration() const {
    return configuration;
}

const std::vector<std::shared_ptr<DeviceCapability>>& RemoteDevice::getCapabilities() const {
    return capabilities;
}

std::span<Device* const> RemoteDevice::getSubDevices() const {
    return subDevicePointers;
}

tfw::utils::void_result RemoteDevice::flashIdentificationLights(uint32_t durationMs) {
    return i2cClient.sendCommand(
        configuration.address,
        configuration.id,
        firmwares::slave::i2c::commands::FlashDeviceIdentificationLights,
        {.durationMs = durationMs}
    );
}

tfw::utils::void_result RemoteDevice::
flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    return i2cClient.sendCommand(
        configuration.address,
        configuration.id,
        firmwares::slave::i2c::commands::FlashButtonIdentificationLight,
        {.buttonNumber = buttonNumber, .durationMs = durationMs}
    );
}

tfw::utils::void_result RemoteDevice::rename(const std::string_view& deviceName) {
    auto params = firmwares::slave::i2c::commands::RenameDeviceParams{};
    strncpy(&params.name[0], deviceName.data(), std::min(deviceName.size(), sizeof(params.name)));

    return i2cClient.sendCommand(
        configuration.address,
        configuration.id,
        firmwares::slave::i2c::commands::RenameDevice,
        params
    );
}

void RemoteDevice::onRemoteEventsAvailable() {
    remoteEventsAvailable = true;
}

bool RemoteDevice::checkPresence() {
    const auto result = i2cClient.readEndpoint(configuration.address, i2c::endpoints::DeviceInformation);

    // Another device answering at the address counts as missing too
    if (!result.has_error && result.value->deviceId == configuration.id) {
        numMissedPresenceChecks = 0;
        return true;
    }

    numMissedPresenceChecks++;
    if (numMissedPresenceChecks < MAX_MISSED_PRESENCE_CHECKS) {
        return true;
    }

    logger->info("Device %08llx missed %u presence checks", configuration.id, numMissedPresenceChecks);
    numMissedPresenceChecks = 0;
    return false;
}

void RemoteDevice::refreshName() {
    const auto result = i2cClient.readEndpoint(configuration.address, i2c::endpoints::DeviceName);
    if (result.has_error) {
        logger->error("Unable to read name of device %08llx", configuration.id);
        return;
    }

    const auto& deviceName = result.value->deviceName;
    configuration.name = std::string(deviceName, strnlen(deviceName, sizeof(deviceName)));
}

void RemoteDevice::refreshSubDevices() {
    for (size_t i = 0; i < subDevices.size(); i++) {
        const auto result = i2cClient.readEndpoint(configuration.address, i2c::endpoints::SubDeviceInformation(i));
        if (result.has_error) {
            logger->error("Unable to read sub-device %u of device %08llx", i, configuration.id);
            continue;
        }

        subDevices[i]->updateInformation(*result.value);
    }
}
