#include "ReadDeviceTypeCommandHandler.h"

ReadDeviceTypeCommandHandler::ReadDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.type"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceTypeCommandHandler::~ReadDeviceTypeCommandHandler() = default;

tfw::utils::void_result ReadDeviceTypeCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    auto deviceId = this->deviceConfigurationManager.getDeviceType();
    if (deviceId == 0) {
        responseWriter.writeLine("0");
    } else {
        responseWriter.writeLine({&deviceId, 1});
    }

    return tfw::utils::void_result::success();
}
