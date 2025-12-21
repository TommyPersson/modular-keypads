#include "SetDeviceTypeCommandHandler.h"

namespace {
    auto logger = common::logging::createLogger("SetDeviceTypeCommandHandler");
}

SetDeviceTypeCommandHandler::SetDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("set.device.type"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceTypeCommandHandler::~SetDeviceTypeCommandHandler() = default;

void SetDeviceTypeCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    if (args.size() != 1) {
        logger->error("execute: wrong number of arguments");
        responseWriter.writeLine("NAK");
        return;
    }

    if (args[0].size() != 1) {
        logger->error("execute: wrong number of characters for device type");
        responseWriter.writeLine("NAK");
        return;
    }

    const auto deviceTypeCode = args[0][0];

    if (!deviceConfigurationManager.setDeviceType(deviceTypeCode)) {
        logger->error("execute: unable to set device type");
        responseWriter.writeLine("NAK");
        return;
    }
}
