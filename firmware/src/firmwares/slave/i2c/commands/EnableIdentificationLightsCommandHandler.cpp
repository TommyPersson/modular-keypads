#include "EnableIdentificationLightsCommandHandler.h"

using namespace firmwares::slave::i2c::commands;

EnableIdentificationLightsCommandHandler::EnableIdentificationLightsCommandHandler(
    devices::DeviceModule& device
) : CommandHandler(EnableIdentificationLights.id),
    device(device) {
}

EnableIdentificationLightsCommandHandler::~EnableIdentificationLightsCommandHandler() = default;

utils::void_result EnableIdentificationLightsCommandHandler::execute(const EnableIdentificationLightsParams* params) {
    device.flashIdentificationLights(params->durationMs);
    return utils::void_result::success();
}
