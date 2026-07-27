#include "DeviceModule.h"

#include <firmwares/slave/i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h>
#include <firmwares/slave/i2c/commands/RenameDeviceRemoteCommandHandler.h>
#include <tfw/hal/logging.h>

#include "firmwares/common/events/RemoteEventTypes.h"
#include "firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceModule");
}


devices::DeviceModule::DeviceModule(
    const DeviceConfiguration& configuration,
    DeviceLocation deviceLocation,
    tfw::hal::i2c::Client& i2cClient
) : configuration(configuration),
    deviceLocation(deviceLocation),
    i2cClient(i2cClient) {
}

void devices::DeviceModule::setup() {
    this->getRuntime().onSwitchEvent().addObserver(this);
    this->getRuntime().onRotaryEncoderEvent().addObserver(this);
}

void devices::DeviceModule::loop() {
    if (deviceLocation == DeviceLocation::Remote) {
        const auto result = i2cClient.readEndpoint(configuration.address, tfw::hal::i2c::endpoints::builtin::Events);
        if (result.has_error) {
            return;
        }

        const auto remoteEvent = *result.value;
        if (remoteEvent.type == 0) {
            return;
        }

        logger->info("event: %02x", remoteEvent.type);

        const auto remoteEventType = static_cast<RemoteEventType>(remoteEvent.type);

        if (remoteEventType == RemoteEventType::BUTTON_PRESSED) {
            logger->info("button pressed event");

            this->deviceSwitchEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .switchNumber = remoteEvent.args[0],
                    .state = tfw::hal::buttons::ButtonState::PRESSED,
                }
            );
        } else if (remoteEventType == RemoteEventType::BUTTON_RELEASED) {
            logger->info("button released event");

            this->deviceSwitchEventSubject.notify(
                {
                    .deviceId = remoteEvent.deviceId,
                    .switchNumber = remoteEvent.args[0],
                    .state = tfw::hal::buttons::ButtonState::UNPRESSED,
                }
            );
        }
    }
}

tfw::utils::void_result devices::DeviceModule::flashIdentificationLights(uint32_t durationMs) {
    if (deviceLocation == DeviceLocation::Remote) {
        return i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::FlashDeviceIdentificationLights,
            {.durationMs = durationMs}
        );
    } else {
        this->getRuntime().flashIdentificationLights(durationMs);
        return tfw::utils::void_result::success();
    }
}

tfw::utils::void_result
devices::DeviceModule::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    if (deviceLocation == DeviceLocation::Remote) {
        return i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::FlashButtonIdentificationLight,
            {.buttonNumber = buttonNumber, .durationMs = durationMs}
        );
    } else {
        this->getRuntime().flashButtonIdentificationLight(buttonNumber, durationMs);
        return tfw::utils::void_result::success();
    }
}

tfw::utils::void_result devices::DeviceModule::rename(const std::string_view& deviceName) {
    if (deviceLocation == DeviceLocation::Remote) {
        auto params = firmwares::slave::i2c::commands::RenameDeviceParams{};
        strncpy(&params.name[0], deviceName.data(), deviceName.length());

        return i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::RenameDevice,
            params
        );
    } else {
        return this->getRuntime().renameDevice(deviceName);
    }
}

void devices::DeviceModule::observe(const DeviceSwitchEvent& event) {
    if (deviceLocation == DeviceLocation::Local) { // TODO rewrite solution to split remote and local devices more clearly. dont rely on register refresing from remote devices.
        deviceSwitchEventSubject.notify(event);
    }
}

void devices::DeviceModule::observe(const DeviceRotaryEncoderEvent& event) {
    if (deviceLocation == DeviceLocation::Local) {
        deviceRotaryEncoderEventSubject.notify(event);
    }
}
