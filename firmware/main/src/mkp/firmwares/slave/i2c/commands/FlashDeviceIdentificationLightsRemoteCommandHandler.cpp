#include "FlashDeviceIdentificationLightsRemoteCommandHandler.h"

#include "TargetDevice.h"

using namespace firmwares::slave::i2c::commands;

FlashDeviceIdentificationLightsRemoteCommandHandler::FlashDeviceIdentificationLightsRemoteCommandHandler(
    mkp::devices::common::LocalDevice& device
) : RemoteCommandHandler(FlashDeviceIdentificationLights.id),
    device(device) {
}

FlashDeviceIdentificationLightsRemoteCommandHandler::~FlashDeviceIdentificationLightsRemoteCommandHandler() = default;

tfw::utils::void_result FlashDeviceIdentificationLightsRemoteCommandHandler::execute(
    const uint64_t targetDeviceId,
    const FlashDeviceIdentificationLightsParams* params
) {
    const auto targetDevice = resolveTargetDevice(device, targetDeviceId);
    if (targetDevice == nullptr) {
        return tfw::utils::void_result::error("device.not.found");
    }

    targetDevice->flashIdentificationLights(params->durationMs);
    return tfw::utils::void_result::success();
}
