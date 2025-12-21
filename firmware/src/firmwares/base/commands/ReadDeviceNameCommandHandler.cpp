#include "ReadDeviceNameCommandHandler.h"

ReadDeviceNameCommandHandler::ReadDeviceNameCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.name"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceNameCommandHandler::~ReadDeviceNameCommandHandler() = default;

void ReadDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    auto name = this->deviceConfigurationManager.getDeviceName();

    responseWriter.writeLine(name);
}
