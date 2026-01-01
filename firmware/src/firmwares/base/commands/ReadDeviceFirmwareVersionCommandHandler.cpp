#include "ReadDeviceFirmwareVersionCommandHandler.h"

ReadDeviceFirmwareVersionCommandHandler::ReadDeviceFirmwareVersionCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.firmware.version"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersionCommandHandler::~ReadDeviceFirmwareVersionCommandHandler() = default;

utils::void_result ReadDeviceFirmwareVersionCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto deviceVersion = this->deviceConfigurationManager.getDeviceVersion();
    if (deviceVersion.empty()) {
        return utils::void_result::error("device.version.not.available");
    }

    responseWriter.writeLine(deviceVersion);

    return utils::void_result::success();
}
