#include "FlashButtonIdentificationLightCommandHandler.h"

#include <tfw/utils/strings.h>

FlashButtonIdentificationLightCommandHandler::FlashButtonIdentificationLightCommandHandler(
    const std::vector<devices::DeviceModule*>& devices
) : CommandHandler("flash.button.identification.light"),
    devices(devices) {
}

FlashButtonIdentificationLightCommandHandler::~FlashButtonIdentificationLightCommandHandler() {
}

tfw::utils::void_result FlashButtonIdentificationLightCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto deviceId = tfw::utils::strings::atou64(args[0], 16);
    const auto buttonNumber = tfw::utils::strings::atou8(args[1], 10);
    const auto durationMs = tfw::utils::strings::atou32(args[2], 10);

    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            device->flashButtonIdentificationLight(buttonNumber, durationMs);
            return tfw::utils::void_result::success();
        }
    }

    return tfw::utils::void_result::error("device.not.found");
}
