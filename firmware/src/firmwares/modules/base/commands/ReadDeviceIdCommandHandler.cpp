#include "ReadDeviceIdCommandHandler.h"


ReadDeviceIdCommandHandler::ReadDeviceIdCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.id", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceIdCommandHandler::~ReadDeviceIdCommandHandler() = default;

void ReadDeviceIdCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId.empty()) {
        responseWriter.writeLine("NAK");
        return;
    }

    responseWriter.writeLine(deviceId);
}
