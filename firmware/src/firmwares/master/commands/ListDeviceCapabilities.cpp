#include "ListDeviceCapabilities.h"

#include <firmwares/modules/common/DeviceModule.h>
#include <utils/strings.h>

ListDeviceCapabilities::ListDeviceCapabilities(
    std::vector<devices::DeviceModule*>& devices
) : CommandHandler("list.device.capabilities"), devices(devices) {
}

ListDeviceCapabilities::~ListDeviceCapabilities() = default;

utils::void_result ListDeviceCapabilities::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto& deviceIdStr = args[0];
    const auto deviceId = utils::strings::atou64(deviceIdStr, 16);

    const devices::DeviceModule* foundDevice = nullptr;
    for (const auto& device : devices) {
        if (device->getConfiguration().id == deviceId) {
            foundDevice = device;
        }
    }

    if (foundDevice == nullptr) {
        return utils::void_result::error("device.not.found");
    }

    const auto& capabilities = foundDevice->getCapabilities();

    for (const auto& capability : capabilities) {
        if (
            const auto pushButton = dynamic_cast<devices::PushButtonCapability*>(capability.get());
            pushButton != nullptr
        ) {
            responseWriter.writeLineF("PushButton,%u,%i", pushButton->number, pushButton->ledIndex);
        }

        if (
            const auto rotaryEncoder = dynamic_cast<devices::RotaryEncoderCapability*>(capability.get());
            rotaryEncoder != nullptr
        ) {
            responseWriter.writeLineF("RotaryEncoder,%u", rotaryEncoder->number);
        }
    }

    return utils::void_result::success();
}
