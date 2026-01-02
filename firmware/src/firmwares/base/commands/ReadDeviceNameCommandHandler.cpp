#include "ReadDeviceNameCommandHandler.h"

ReadDeviceNameCommandHandler::ReadDeviceNameCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.name"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceNameCommandHandler::~ReadDeviceNameCommandHandler() = default;

utils::void_result ReadDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
    ) {
    auto name = this->deviceConfigurationManager.getDeviceName();

    responseWriter.writeLine(name);

    return utils::void_result::success();
}
