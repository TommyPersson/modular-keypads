#include "DeviceModule.h"

#include "firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"

devices::DeviceModule::DeviceModule(
    DeviceMode deviceMode,
    utils::i2c::Client& i2cClient
) : deviceMode(deviceMode),
    i2cClient(i2cClient) {
}


void devices::DeviceModule::flashIdentificationLights(uint32_t durationMs) {
    if (deviceMode == DeviceMode::Remote) {
        i2cClient.sendCommand(
            this->getConfiguration().address,
            firmwares::slave::i2c::commands::FlashDeviceIdentificationLights,
            {.durationMs = durationMs}
        );
    } else {
        this->getRuntime().flashIdentificationLights(durationMs);
    }
}
