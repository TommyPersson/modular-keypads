#include "FlashDeviceIdentificationLightsCommandHandler.h"

#include "../../../tfw/utils/strings.h"

FlashDeviceIdentificationLightsCommandHandler::FlashDeviceIdentificationLightsCommandHandler(
    const std::vector<devices::DeviceModule*>& devices
) : CommandHandler("flash.identification.lights"),
    devices(devices) {
}

FlashDeviceIdentificationLightsCommandHandler::~FlashDeviceIdentificationLightsCommandHandler() {
}

tfw::utils::void_result FlashDeviceIdentificationLightsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto deviceId = tfw::utils::strings::atou64(args[0], 16);
    const auto durationMs = tfw::utils::strings::atou32(args[1], 10);

    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            device->flashIdentificationLights(durationMs);
            return tfw::utils::void_result::success();
        }
    }

    return tfw::utils::void_result::error("device.not.found");
}
