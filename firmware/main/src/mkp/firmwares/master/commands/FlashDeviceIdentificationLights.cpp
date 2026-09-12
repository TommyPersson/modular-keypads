#include "FlashDeviceIdentificationLights.h"

#include <tfw/utils/strings.h>

using namespace mkp::firmwares::master::commands;

FlashDeviceIdentificationLights::FlashDeviceIdentificationLights(
    const std::vector<mkp::devices::common::Device*>& devices
) : CommandHandler("flash.identification.lights"),
    devices(devices) {
}

FlashDeviceIdentificationLights::~FlashDeviceIdentificationLights() {
}

tfw::utils::void_result FlashDeviceIdentificationLights::execute(
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
