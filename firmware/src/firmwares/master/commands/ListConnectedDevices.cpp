#include "ListConnectedDevices.h"

ListConnectedDevices::ListConnectedDevices(
    std::vector<devices::DeviceModule*>& devices
) : CommandHandler("list.connected.devices"),
    devices(devices) {}

ListConnectedDevices::~ListConnectedDevices() = default;

void ListConnectedDevices::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    for (auto& device : devices) {
        const auto& config = device->getConfiguration();
        responseWriter.writeLineF("%08llx,%02x,%c,%s,", config.id, config.address, config.type, config.name.c_str());
    }
}
