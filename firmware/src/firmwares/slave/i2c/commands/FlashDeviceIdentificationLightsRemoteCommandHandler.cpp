#include "FlashDeviceIdentificationLightsRemoteCommandHandler.h"

using namespace firmwares::slave::i2c::commands;

FlashDeviceIdentificationLightsRemoteCommandHandler::FlashDeviceIdentificationLightsRemoteCommandHandler(
    devices::DeviceModule& device
) : RemoteCommandHandler(FlashDeviceIdentificationLights.id),
    device(device) {
}

FlashDeviceIdentificationLightsRemoteCommandHandler::~FlashDeviceIdentificationLightsRemoteCommandHandler() = default;

utils::void_result FlashDeviceIdentificationLightsRemoteCommandHandler::execute(const FlashDeviceIdentificationLightsParams* params) {
    device.flashIdentificationLights(params->durationMs);
    return utils::void_result::success();
}
