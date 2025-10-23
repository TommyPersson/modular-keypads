#include "ReadDeviceTypeCommandHandler.h"

ReadDeviceTypeCommandHandler::ReadDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.type", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceTypeCommandHandler::~ReadDeviceTypeCommandHandler() = default;

void ReadDeviceTypeCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    auto deviceId = this->deviceConfigurationManager.getDeviceType();
    if (deviceId == 0) {
        responseWriter.writeLine("0");
        return;
    }

    responseWriter.writeLine({&deviceId, 1});
}
