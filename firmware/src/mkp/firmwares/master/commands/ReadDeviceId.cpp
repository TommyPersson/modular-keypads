#include "ReadDeviceId.h"

#include <tfw/utils/allocations.h>

using namespace mkp::firmwares::master::commands;

ReadDeviceId::ReadDeviceId(
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.id"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceId::~ReadDeviceId() = default;

tfw::utils::void_result ReadDeviceId::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId == 0) {
        return tfw::utils::void_result::error("unable.to.read.device.id");
    }

    responseWriter.writeLineF("%08llx", deviceId);

    return tfw::utils::void_result::success();
}
