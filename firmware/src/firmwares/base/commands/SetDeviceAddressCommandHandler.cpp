#include "SetDeviceAddressCommandHandler.h"

#include "utils/strings.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("SetDeviceAddressCommandHandler");
}

SetDeviceAddressCommandHandler::SetDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("set.device.address"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceAddressCommandHandler::~SetDeviceAddressCommandHandler() = default;

utils::void_result SetDeviceAddressCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto addressStr = args[0];

    const auto address = utils::strings::atol(addressStr, 16);

    if (!this->deviceConfigurationManager.setDeviceAddress(address)) {
        return utils::void_result::error("unable.to.set.device.address");
    }

    return utils::void_result::success();
}
