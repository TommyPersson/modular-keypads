#include "ReadDeviceIdCommandHandler.h"

#include <utils/allocations/ArenaUtils.h>


ReadDeviceIdCommandHandler::ReadDeviceIdCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.id"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceIdCommandHandler::~ReadDeviceIdCommandHandler() = default;

utils::void_result ReadDeviceIdCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId == 0) {
        return utils::void_result::error("unable.to.read.device.id");
    }

    responseWriter.writeLineF("%08llx", deviceId);

    return utils::void_result::success();
}
