#include "ListConnectedDevices.h"

#include "tfw/hal/logging/Logger.h"

using namespace mkp::firmwares::master::commands;

namespace {
    const auto logger = tfw::hal::logging::createLogger("ListConnectedDevices");
}

ListConnectedDevices::ListConnectedDevices(
    std::vector<mkp::devices::common::Device*>& devices
) : CommandHandler("list.connected.devices"),
    devices(devices) {
}

ListConnectedDevices::~ListConnectedDevices() = default;

tfw::utils::void_result ListConnectedDevices::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    for (auto& device : devices) {
        const auto& config = device->getConfiguration();
        if (config.id != 0) {
            responseWriter.writeLineF(
                "%08llx,%02x,%s,%s,%08llx,%u",
                config.id,
                config.address,
                config.type.c_str(),
                config.name.c_str(),
                config.parentDeviceId,
                config.subDeviceSlot
            );
        }
    }

    return tfw::utils::void_result::success();
}
