#include "ReadDeviceAddressCommandHandler.h"

#include <utils/allocations/ArenaUtils.h>

ReadDeviceAddressCommandHandler::ReadDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.address"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceAddressCommandHandler::~ReadDeviceAddressCommandHandler() = default;

void ReadDeviceAddressCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    auto address = this->deviceConfigurationManager.getDeviceAddress();
    auto addressStr = arena::strings::sprintf(arena, "0x%02x", address);

    responseWriter.writeLine(addressStr);
}
