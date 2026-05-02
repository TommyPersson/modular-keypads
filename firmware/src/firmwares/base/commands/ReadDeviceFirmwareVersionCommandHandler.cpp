#include "ReadDeviceFirmwareVersionCommandHandler.h"

ReadDeviceFirmwareVersionCommandHandler::ReadDeviceFirmwareVersionCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.firmware.version"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersionCommandHandler::~ReadDeviceFirmwareVersionCommandHandler() = default;

tfw::utils::void_result ReadDeviceFirmwareVersionCommandHandler::execute(
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
