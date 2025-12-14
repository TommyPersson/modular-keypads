#include "SetDeviceNameCommandHandler.h"

#include <utils/strings.h>

namespace {
    auto logger = common::logging::createLogger("SetDeviceNameCommandHandler");
}

SetDeviceNameCommandHandler::SetDeviceNameCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("set.device.name"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceNameCommandHandler::~SetDeviceNameCommandHandler() = default;

void SetDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto name = args[0];

    if (!this->deviceConfigurationManager.setDeviceName(name)) {
        logger->error("execute: unable to set device name");
        responseWriter.writeLine("NAK");
    }
}
