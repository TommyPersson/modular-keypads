#include "ReadDeviceAddressCommandHandler.h"

#include <tfw/utils/allocations.h>

ReadDeviceAddressCommandHandler::ReadDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.address"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceAddressCommandHandler::~ReadDeviceAddressCommandHandler() = default;

tfw::utils::void_result ReadDeviceAddressCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    auto address = this->deviceConfigurationManager.getDeviceAddress();
    auto addressStr = tfw::utils::allocations::arena::strings::sprintf(arena, "0x%02x", address);

    responseWriter.writeLine(addressStr);

    return tfw::utils::void_result::success();
}
