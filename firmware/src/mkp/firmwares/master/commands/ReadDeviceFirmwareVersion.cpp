#include "ReadDeviceFirmwareVersion.h"

using namespace mkp::firmwares::master::commands;

ReadDeviceFirmwareVersion::ReadDeviceFirmwareVersion(
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.firmware.version"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersion::~ReadDeviceFirmwareVersion() = default;

tfw::utils::void_result ReadDeviceFirmwareVersion::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto deviceVersion = this->deviceConfigurationManager.getDeviceVersion();
    if (deviceVersion.empty()) {
        return tfw::utils::void_result::error("device.version.not.available");
    }

    responseWriter.writeLine(deviceVersion);

    return tfw::utils::void_result::success();
}
