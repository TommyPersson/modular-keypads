#include "SetDeviceAddressCommandHandler.h"

#include "../../../tfw/utils/strings.h"
#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::utils::logging::createLogger("SetDeviceAddressCommandHandler");
}

SetDeviceAddressCommandHandler::SetDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("set.device.address"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceAddressCommandHandler::~SetDeviceAddressCommandHandler() = default;

tfw::utils::void_result SetDeviceAddressCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto addressStr = args[0];

    const auto address = tfw::utils::strings::atol(addressStr, 16);

    if (!this->deviceConfigurationManager.setDeviceAddress(address)) {
        return tfw::utils::void_result::error("unable.to.set.device.address");
    }

    return tfw::utils::void_result::success();
}
