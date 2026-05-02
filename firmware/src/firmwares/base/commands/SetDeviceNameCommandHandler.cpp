#include "SetDeviceNameCommandHandler.h"

#include <tfw/utils/strings.h>
#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::utils::logging::createLogger("SetDeviceNameCommandHandler");
}

SetDeviceNameCommandHandler::SetDeviceNameCommandHandler(
    std::vector<devices::DeviceModule*>& devices
) : CommandHandler("set.device.name"),
    devices(devices) {
}

SetDeviceNameCommandHandler::~SetDeviceNameCommandHandler() = default;

tfw::utils::void_result SetDeviceNameCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto deviceId = tfw::utils::strings::atou64(args[0], 16);
    const auto deviceName = args[1];

    if (deviceName.size() > 15) {
        return tfw::utils::void_result::error("device.name.too.long");
    }

    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            return device->rename(deviceName);
        }
    }

    return tfw::utils::void_result::success();
}
