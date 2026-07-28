#include "RemoteDevice.h"

#include <tfw/hal/logging/Logger.h>

#include "events/RemoteEventTypes.h"
#include "firmwares/modules/a/DeviceModuleA.h"
#include "firmwares/modules/m/DeviceModuleM.h"
#include "firmwares/slave/i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h"
#include "firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"
#include "firmwares/slave/i2c/commands/RenameDeviceRemoteCommandHandler.h"


namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceModule");

    std::vector<std::shared_ptr<devices::DeviceCapability>> noCapabilities;
}

devices::RemoteDevice::RemoteDevice(
    const DeviceConfiguration& configuration,
    const NotifierFactory& notifierFactory,
    tfw::hal::i2c::Client& i2cClient
) : configuration(configuration),
    i2cClient(i2cClient) {
    notifier = notifierFactory.create(configuration.id);
    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(*notifier);
    deviceSwitchEventSubject.addObserver(switchStateChangeNotifier.get());
}

devices::RemoteDevice::~RemoteDevice() {
    deviceSwitchEventSubject.removeObserver(switchStateChangeNotifier.get());
}

void devices::RemoteDevice::setup() {
}

void devices::RemoteDevice::loop() {
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
    }
}

const DeviceConfiguration& devices::RemoteDevice::getConfiguration() const {
    return configuration;
}

const std::vector<std::shared_ptr<devices::DeviceCapability>>& devices::RemoteDevice::getCapabilities() const {
    switch (configuration.type) {
    case 'a': return a::capabilities;
    case 'm': return m::capabilities;
    default: return noCapabilities;
    }
}

tfw::utils::void_result devices::RemoteDevice::flashIdentificationLights(uint32_t durationMs) {
    return i2cClient.sendCommand(
        this->getConfiguration().address,
        firmwares::slave::i2c::commands::FlashDeviceIdentificationLights,
        {.durationMs = durationMs}
    );
}

tfw::utils::void_result devices::RemoteDevice::
flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    return i2cClient.sendCommand(
        this->getConfiguration().address,
        firmwares::slave::i2c::commands::FlashButtonIdentificationLight,
        {.buttonNumber = buttonNumber, .durationMs = durationMs}
    );
}

tfw::utils::void_result devices::RemoteDevice::rename(const std::string_view& deviceName) {
    auto params = firmwares::slave::i2c::commands::RenameDeviceParams{};
    strncpy(&params.name[0], deviceName.data(), sizeof(params.name));

    return i2cClient.sendCommand(
        this->getConfiguration().address,
        firmwares::slave::i2c::commands::RenameDevice,
        params
    );
}
