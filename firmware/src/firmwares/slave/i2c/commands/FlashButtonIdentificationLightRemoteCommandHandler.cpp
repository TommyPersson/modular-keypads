#include "FlashButtonIdentificationLightRemoteCommandHandler.h"

using namespace firmwares::slave::i2c::commands;

FlashButtonIdentificationLightRemoteCommandHandler::FlashButtonIdentificationLightRemoteCommandHandler(
    devices::DeviceModule& device
) : RemoteCommandHandler(FlashButtonIdentificationLight.id),
    device(device) {
}

FlashButtonIdentificationLightRemoteCommandHandler::~FlashButtonIdentificationLightRemoteCommandHandler() = default;

tfw::utils::void_result FlashButtonIdentificationLightRemoteCommandHandler::execute(const FlashButtonIdentificationLightParams* params) {
    device.flashButtonIdentificationLight(params->buttonNumber, params->durationMs);
    return tfw::utils::void_result::success();
}
