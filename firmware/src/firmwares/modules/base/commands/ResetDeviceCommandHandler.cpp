#include "ResetDeviceCommandHandler.h"

ResetDeviceCommandHandler::ResetDeviceCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("reset.device", logger),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ResetDeviceCommandHandler::~ResetDeviceCommandHandler() = default;

std::string ResetDeviceCommandHandler::execute(const std::span<const std::string_view>& args) {
    ESP.restart();
    return "ACK";
}
