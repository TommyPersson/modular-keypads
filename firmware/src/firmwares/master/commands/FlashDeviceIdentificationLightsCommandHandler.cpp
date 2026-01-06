#include "FlashDeviceIdentificationLightsCommandHandler.h"

#include "utils/strings.h"

FlashDeviceIdentificationLightsCommandHandler::FlashDeviceIdentificationLightsCommandHandler(
    const std::vector<devices::DeviceModule*>& devices
) : CommandHandler("flash.identification.lights"),
    devices(devices) {
}

FlashDeviceIdentificationLightsCommandHandler::~FlashDeviceIdentificationLightsCommandHandler() {
}

utils::void_result FlashDeviceIdentificationLightsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto deviceId = utils::strings::atou64(args[0], 16);
    const auto durationMs = utils::strings::atou32(args[1], 10);

    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            device->flashIdentificationLights(durationMs);
            return utils::void_result::success();
        }
    }

    return utils::void_result::error("device.not.found");
}
