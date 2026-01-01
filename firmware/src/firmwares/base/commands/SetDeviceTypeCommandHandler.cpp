#include "SetDeviceTypeCommandHandler.h"

namespace {
    auto logger = common::logging::createLogger("SetDeviceTypeCommandHandler");
}

SetDeviceTypeCommandHandler::SetDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("set.device.type"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceTypeCommandHandler::~SetDeviceTypeCommandHandler() = default;

utils::void_result SetDeviceTypeCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    if (args.size() != 1) {
        return utils::void_result::error("incorrect.number.of.arguments");
    }

    if (args[0].size() != 1) {
        return utils::void_result::error("incorrect.argument.length");
    }

    const auto deviceTypeCode = args[0][0];

    if (!deviceConfigurationManager.setDeviceType(deviceTypeCode)) {
        return utils::void_result::error("unable.to.set.device.type");
    }

    return utils::void_result::success();
}
