#include "ResetDeviceCommandHandler.h"

ResetDeviceCommandHandler::ResetDeviceCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("reset.device"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ResetDeviceCommandHandler::~ResetDeviceCommandHandler() = default;

utils::void_result ResetDeviceCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    ESP.restart();

    return utils::void_result::success();
}
