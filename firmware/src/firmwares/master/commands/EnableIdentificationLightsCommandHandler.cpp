#include "EnableIdentificationLightsCommandHandler.h"

#include <utils/strings.h>

EnableIdentificationLightsCommandHandler::EnableIdentificationLightsCommandHandler(
    const std::vector<devices::DeviceModule*>& devices
) : CommandHandler("enable.identification.lights"),
    devices(devices) {
}

EnableIdentificationLightsCommandHandler::~EnableIdentificationLightsCommandHandler() {
}

utils::void_result EnableIdentificationLightsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto deviceId = utils::strings::atou64(args[0], 16);

    const auto durationMs = 3'000;


    for (const auto device : devices) {
        if (device->getConfiguration().id == deviceId) {
            device->flashIdentificationLights(durationMs);
            return utils::void_result::success();
        }
    }

    return utils::void_result::error("device.not.found");
}
