#include "RenameDeviceRemoteCommandHandler.h"

using namespace firmwares::slave::i2c::commands;

RenameDeviceRemoteCommandHandler::RenameDeviceRemoteCommandHandler(
    devices::DeviceModule& device
) : RemoteCommandHandler(RenameDevice.id),
    device(device) {
}

RenameDeviceRemoteCommandHandler::~RenameDeviceRemoteCommandHandler() = default;

utils::void_result RenameDeviceRemoteCommandHandler::execute(const RenameDeviceParams* params) {
    const auto name = std::string_view(params->name, sizeof(params->name));
    return device.rename(name);
}
