#include "ReadDeviceNameCommandHandler.h"

ReadDeviceNameCommandHandler::ReadDeviceNameCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.name"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceNameCommandHandler::~ReadDeviceNameCommandHandler() = default;

tfw::utils::void_result ReadDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
    ) {
    auto name = this->deviceConfigurationManager.getDeviceName();

    responseWriter.writeLine(name);

    return tfw::utils::void_result::success();
}
