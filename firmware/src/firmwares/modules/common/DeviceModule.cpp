#include "DeviceModule.h"

#include <firmwares/slave/i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h>
#include <firmwares/slave/i2c/commands/RenameDeviceRemoteCommandHandler.h>
#include <utils/logging/Logger.h>

#include "firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceModule");
}

devices::DeviceModule::DeviceModule(
    DeviceLocation deviceLocation,
    utils::i2c::Client& i2cClient
) : deviceLocation(deviceLocation),
    i2cClient(i2cClient) {
}

utils::void_result devices::DeviceModule::flashIdentificationLights(uint32_t durationMs) {
    if (deviceLocation == DeviceLocation::Remote) {
        return i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::FlashDeviceIdentificationLights,
            {.durationMs = durationMs}
        );
    } else {
        this->getRuntime().flashIdentificationLights(durationMs);
        return utils::void_result::success();
    }
}

utils::void_result devices::DeviceModule::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    if (deviceLocation == DeviceLocation::Remote) {
        return i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::FlashButtonIdentificationLight,
            {.buttonNumber = buttonNumber, .durationMs = durationMs}
        );
    } else {
        this->getRuntime().flashButtonIdentificationLight(buttonNumber, durationMs);
        return utils::void_result::success();
    }
}

utils::void_result devices::DeviceModule::rename(const std::string_view& deviceName) {
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
