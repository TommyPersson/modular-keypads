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
        subDevices.push_back(std::make_unique<RemoteSubDevice>(configuration, i2cClient));
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
