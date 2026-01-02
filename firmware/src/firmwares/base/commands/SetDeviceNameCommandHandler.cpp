#include "SetDeviceNameCommandHandler.h"

#include "utils/strings.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("SetDeviceNameCommandHandler");
}

SetDeviceNameCommandHandler::SetDeviceNameCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("set.device.name"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceNameCommandHandler::~SetDeviceNameCommandHandler() = default;

utils::void_result SetDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto name = args[0];

    if (name.size() > 15) {
        return utils::void_result::error("device.name.too.long");
    }

    if (!this->deviceConfigurationManager.setDeviceName(name)) {
        return utils::void_result::error("unable.to.set.device.name");
    }

    return utils::void_result::success();
}
