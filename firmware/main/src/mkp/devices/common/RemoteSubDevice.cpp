#include "RemoteSubDevice.h"

#include "SubDeviceCapabilities.h"

#include "mkp/firmwares/slave/i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h"
#include "mkp/firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"
#include "mkp/firmwares/slave/i2c/commands/RenameDeviceRemoteCommandHandler.h"

using namespace mkp::devices::common;

RemoteSubDevice::RemoteSubDevice(
    const DeviceConfiguration& parentConfiguration,
    tfw::hal::i2c::Client& i2cClient
) : configuration{
        .id = 0,
        .name = "",
        .type = "0",
        .address = parentConfiguration.address,
        .parentDeviceId = parentConfiguration.id,
    },
    i2cClient(i2cClient) {
}

RemoteSubDevice::~RemoteSubDevice() = default;

void RemoteSubDevice::setup() {
}

void RemoteSubDevice::loop() {
}

const DeviceConfiguration& RemoteSubDevice::getConfiguration() const {
    return configuration;
}

const std::vector<std::shared_ptr<DeviceCapability>>& RemoteSubDevice::getCapabilities() const {
    return subDeviceCapabilitiesFor(configuration.type);
}

tfw::utils::void_result RemoteSubDevice::flashIdentificationLights(uint32_t durationMs) {
    if (configuration.id == 0) {
        return tfw::utils::void_result::error("sub.device.not.connected");
    }

    return i2cClient.sendCommand(
        configuration.address,
        configuration.id,
        firmwares::slave::i2c::commands::FlashDeviceIdentificationLights,
        {.durationMs = durationMs}
    );
}

tfw::utils::void_result RemoteSubDevice::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    if (configuration.id == 0) {
        return tfw::utils::void_result::error("sub.device.not.connected");
    }

    return i2cClient.sendCommand(
        configuration.address,
        configuration.id,
        firmwares::slave::i2c::commands::FlashButtonIdentificationLight,
        {.buttonNumber = buttonNumber, .durationMs = durationMs}
    );
}

tfw::utils::void_result RemoteSubDevice::rename(const std::string_view& deviceName) {
    if (configuration.id == 0) {
        return tfw::utils::void_result::error("sub.device.not.connected");
    }

    auto params = firmwares::slave::i2c::commands::RenameDeviceParams{};
    strncpy(&params.name[0], deviceName.data(), std::min(deviceName.size(), sizeof(params.name)));

    const auto result = i2cClient.sendCommand(
        configuration.address,
        configuration.id,
        firmwares::slave::i2c::commands::RenameDevice,
        params
    );

    // The slave updates the sub-device's endpoint asynchronously, so it isn't read back here
    if (!result.has_error) {
        configuration.name = std::string(deviceName);
    }

    return result;
}

void RemoteSubDevice::updateInformation(const i2c::structs::SubDeviceInformation& information) {
    configuration.id = information.deviceId;

    if (information.deviceId == 0) {
        configuration.type = "0";
        configuration.name = "";
    } else {
        configuration.type = std::string(
            information.deviceType,
            strnlen(information.deviceType, sizeof(information.deviceType))
        );
        configuration.name = std::string(
            information.deviceName,
            strnlen(information.deviceName, sizeof(information.deviceName))
        );
    }
}
