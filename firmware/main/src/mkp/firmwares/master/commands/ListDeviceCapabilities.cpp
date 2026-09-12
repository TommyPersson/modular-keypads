#include "ListDeviceCapabilities.h"

#include <tfw/utils/strings.h>

#include "mkp/devices/common/LocalDevice.h"

using namespace mkp::firmwares::master::commands;

ListDeviceCapabilities::ListDeviceCapabilities(
    std::vector<mkp::devices::common::Device*>& devices
) : CommandHandler("list.device.capabilities"), devices(devices) {
}

ListDeviceCapabilities::~ListDeviceCapabilities() = default;

tfw::utils::void_result ListDeviceCapabilities::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto& deviceIdStr = args[0];
    const auto deviceId = tfw::utils::strings::atou64(deviceIdStr, 16);

    const mkp::devices::common::Device* foundDevice = nullptr;
    for (const auto& device : devices) {
        if (device->getConfiguration().id == deviceId) {
            foundDevice = device;
        }
    }

    if (foundDevice == nullptr) {
        return tfw::utils::void_result::error("device.not.found");
    }

    const auto& capabilities = foundDevice->getCapabilities();

    for (const auto& capability : capabilities) {
        if (
            const auto pushButton = dynamic_cast<mkp::devices::common::PushButtonCapability*>(capability.get());
            pushButton != nullptr
        ) {
            responseWriter.writeLineF("PushButton,%u,%i", pushButton->number, pushButton->ledIndex);
        }

        if (
            const auto rotaryEncoder = dynamic_cast<mkp::devices::common::RotaryEncoderCapability*>(capability.get());
            rotaryEncoder != nullptr
        ) {
            responseWriter.writeLineF("RotaryEncoder,%u", rotaryEncoder->number);
        }
    }

    return tfw::utils::void_result::success();
}
