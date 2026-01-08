#include "FlashButtonIdentificationLightCommandHandler.h"

#include "utils/strings.h"

FlashButtonIdentificationLightCommandHandler::FlashButtonIdentificationLightCommandHandler(
    const std::vector<devices::DeviceModule*>& devices
) : CommandHandler("flash.button.identification.light"),
    devices(devices) {
}

FlashButtonIdentificationLightCommandHandler::~FlashButtonIdentificationLightCommandHandler() {
}

utils::void_result FlashButtonIdentificationLightCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto deviceId = utils::strings::atou64(args[0], 16);
    const auto buttonNumber = utils::strings::atou8(args[1], 10);
    const auto durationMs = utils::strings::atou32(args[2], 10);

    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            device->flashButtonIdentificationLight(buttonNumber, durationMs);
            return utils::void_result::success();
        }
    }

    return utils::void_result::error("device.not.found");
}
