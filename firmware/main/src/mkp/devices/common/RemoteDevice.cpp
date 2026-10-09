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
    tfw::hal::i2c::Client& i2cClient
) : configuration(configuration),
    capabilities(capabilities),
    i2cClient(i2cClient) {
}

RemoteDevice::~RemoteDevice() = default;

void RemoteDevice::setup() {
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
            this->subDeviceConnectedSubject.notify({.deviceId = remoteEvent.deviceId});
        } else if (remoteEventType == RemoteEventType::SUB_DEVICE_DISCONNECTED) {
            this->subDeviceDisconnectedSubject.notify({.deviceId = remoteEvent.deviceId});
        }
    }
}

const DeviceConfiguration& RemoteDevice::getConfiguration() const {
    return configuration;
}

const std::vector<std::shared_ptr<DeviceCapability>>& RemoteDevice::getCapabilities() const {
    return capabilities;
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
    strncpy(&params.name[0], deviceName.data(), sizeof(params.name));

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
