#include "SetDeviceTypeCommandHandler.h"

SetDeviceTypeCommandHandler::SetDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("set.device.type", logger),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceTypeCommandHandler::~SetDeviceTypeCommandHandler() = default;

void SetDeviceTypeCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    if (args.size() != 1) {
        this->logger.error("SetDeviceTypeCommandHandler::execute: wrong number of arguments");
        responseWriter.writeLine("NAK");
        return;
    }

    if (args[0].size() != 1) {
        this->logger.error("SetDeviceTypeCommandHandler::execute: wrong number of characters for device type");
        responseWriter.writeLine("NAK");
        return;
    }

    const auto deviceTypeCode = args[0][0];

    if (!deviceConfigurationManager.setDeviceType(deviceTypeCode)) {
        this->logger.error("SetDeviceTypeCommandHandler::execute: unable to set device type");
        responseWriter.writeLine("NAK");
        return;
    }
}
