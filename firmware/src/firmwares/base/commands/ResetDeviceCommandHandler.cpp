#include "ResetDeviceCommandHandler.h"

ResetDeviceCommandHandler::ResetDeviceCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("reset.device"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ResetDeviceCommandHandler::~ResetDeviceCommandHandler() = default;

tfw::utils::void_result ResetDeviceCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    ESP.restart();

    return tfw::utils::void_result::success();
}
