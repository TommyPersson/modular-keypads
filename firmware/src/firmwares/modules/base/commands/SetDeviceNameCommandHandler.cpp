#include "SetDeviceNameCommandHandler.h"

#include <utils/strings.h>

SetDeviceNameCommandHandler::SetDeviceNameCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("set.device.name", logger),
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
        this->logger.error("SetDeviceNameCommandHandler::execute: unable to set device name");
        responseWriter.writeLine("NAK");
    }
}
