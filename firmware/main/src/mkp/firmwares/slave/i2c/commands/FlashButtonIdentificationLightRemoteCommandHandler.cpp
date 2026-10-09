#include "FlashButtonIdentificationLightRemoteCommandHandler.h"

#include "TargetDevice.h"

using namespace firmwares::slave::i2c::commands;

FlashButtonIdentificationLightRemoteCommandHandler::FlashButtonIdentificationLightRemoteCommandHandler(
    mkp::devices::common::LocalDevice& device
) : RemoteCommandHandler(FlashButtonIdentificationLight.id),
    device(device) {
}

FlashButtonIdentificationLightRemoteCommandHandler::~FlashButtonIdentificationLightRemoteCommandHandler() = default;

tfw::utils::void_result FlashButtonIdentificationLightRemoteCommandHandler::execute(
    const uint64_t targetDeviceId,
    const FlashButtonIdentificationLightParams* params
) {
    const auto targetDevice = resolveTargetDevice(device, targetDeviceId);
    if (targetDevice == nullptr) {
        return tfw::utils::void_result::error("device.not.found");
    }

    targetDevice->flashButtonIdentificationLight(params->buttonNumber, params->durationMs);
    return tfw::utils::void_result::success();
}
