#include "ReadDeviceIdCommandHandler.h"

#include <utils/allocations/ArenaUtils.h>


ReadDeviceIdCommandHandler::ReadDeviceIdCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("read.device.id"),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceIdCommandHandler::~ReadDeviceIdCommandHandler() = default;

void ReadDeviceIdCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId == 0) {
        responseWriter.writeLine("NAK");
        return;
    }

    responseWriter.writeLine(arena::strings::sprintf(arena, "%08llx", deviceId));
}
