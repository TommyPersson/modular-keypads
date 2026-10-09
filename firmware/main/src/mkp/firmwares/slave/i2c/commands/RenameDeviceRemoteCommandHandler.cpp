#include "RenameDeviceRemoteCommandHandler.h"

#include "TargetDevice.h"

using namespace firmwares::slave::i2c::commands;

RenameDeviceRemoteCommandHandler::RenameDeviceRemoteCommandHandler(
    mkp::devices::common::LocalDevice& device
) : RemoteCommandHandler(RenameDevice.id),
    device(device) {
}

RenameDeviceRemoteCommandHandler::~RenameDeviceRemoteCommandHandler() = default;

tfw::utils::void_result RenameDeviceRemoteCommandHandler::execute(
    const uint64_t targetDeviceId,
    const RenameDeviceParams* params
) {
    const auto targetDevice = resolveTargetDevice(device, targetDeviceId);
    if (targetDevice == nullptr) {
        return tfw::utils::void_result::error("device.not.found");
    }

    // Trimmed, as sub-devices would otherwise keep the padding as part of their name
    const auto name = std::string_view(params->name, strnlen(params->name, sizeof(params->name)));
    return targetDevice->rename(name);
}
