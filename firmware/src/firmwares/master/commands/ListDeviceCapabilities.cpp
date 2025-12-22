#include "ListDeviceCapabilities.h"

#include <firmwares/modules/common/DeviceModule.h>
#include <utils/strings.h>

ListDeviceCapabilities::ListDeviceCapabilities(
    std::vector<devices::DeviceModule*>& devices
) : CommandHandler("list.device.capabilities"), devices(devices) {}

ListDeviceCapabilities::~ListDeviceCapabilities() = default;

void ListDeviceCapabilities::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
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
        return;
    }

    const auto& capabilities = foundDevice->getCapabilities();

    for (const auto& capability : capabilities) {
        if (
            const auto pushButton = dynamic_cast<devices::PushButtonCapability*>(capability.get());
            pushButton != nullptr
        ) {
            responseWriter.writeLineF("PushButton,%i", pushButton->number);
        }

        if (
            const auto rotaryEncoder = dynamic_cast<devices::RotaryEncoderCapability*>(capability.get());
            rotaryEncoder != nullptr
        ) {
            responseWriter.writeLineF("RotaryEncoder,%i", rotaryEncoder->number);
        }
    }
}
