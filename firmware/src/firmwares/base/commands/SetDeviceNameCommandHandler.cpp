#include "SetDeviceNameCommandHandler.h"

#include "utils/strings.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("SetDeviceNameCommandHandler");
}

SetDeviceNameCommandHandler::SetDeviceNameCommandHandler(
    std::vector<devices::DeviceModule*>& devices
) : CommandHandler("set.device.name"),
    devices(devices) {
}

SetDeviceNameCommandHandler::~SetDeviceNameCommandHandler() = default;

utils::void_result SetDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto deviceId = utils::strings::atou64(args[0], 16);
    const auto deviceName = args[1];

    if (deviceName.size() > 15) {
        return utils::void_result::error("device.name.too.long");
    }

    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            return device->rename(deviceName);
        }
    }

    return utils::void_result::success();
}
