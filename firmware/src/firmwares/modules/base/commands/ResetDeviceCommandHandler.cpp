#include "ResetDeviceCommandHandler.h"

ResetDeviceCommandHandler::ResetDeviceCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("reset.device", logger),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ResetDeviceCommandHandler::~ResetDeviceCommandHandler() = default;

void ResetDeviceCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    ESP.restart();
    responseWriter.writeLine("ACK");
}
