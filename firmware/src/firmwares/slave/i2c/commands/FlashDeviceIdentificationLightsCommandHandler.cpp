#include "FlashDeviceIdentificationLightsCommandHandler.h"

using namespace firmwares::slave::i2c::commands;

FlashDeviceIdentificationLightsCommandHandler::FlashDeviceIdentificationLightsCommandHandler(
    devices::DeviceModule& device
) : CommandHandler(FlashDeviceIdentificationLights.id),
    device(device) {
}

FlashDeviceIdentificationLightsCommandHandler::~FlashDeviceIdentificationLightsCommandHandler() = default;

utils::void_result FlashDeviceIdentificationLightsCommandHandler::execute(const FlashDeviceIdentificationLightsParams* params) {
    device.flashIdentificationLights(params->durationMs);
    return utils::void_result::success();
}
