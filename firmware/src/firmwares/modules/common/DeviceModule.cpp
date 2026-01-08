#include "DeviceModule.h"

#include <firmwares/slave/i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h>
#include <firmwares/slave/i2c/commands/RenameDeviceRemoteCommandHandler.h>
#include <utils/logging/Logger.h>

#include "firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceModule");
}

devices::DeviceModule::DeviceModule(
    DeviceMode deviceMode,
    utils::i2c::Client& i2cClient
) : deviceMode(deviceMode),
    i2cClient(i2cClient) {
}

utils::void_result devices::DeviceModule::flashIdentificationLights(uint32_t durationMs) {
    if (deviceMode == DeviceMode::Remote) {
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
    if (deviceMode == DeviceMode::Remote) {
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
    if (deviceMode == DeviceMode::Remote) {
        auto params = firmwares::slave::i2c::commands::RenameDeviceParams{};
        strncpy(&params.name[0], deviceName.data(), deviceName.length());

        return i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::RenameDevice,
            params
        );
    } else {
        //logger->info("pretend to rename to '%.*s'", deviceName.length(), deviceName.data());
        return this->getRuntime().renameDevice(deviceName);
    }
}
