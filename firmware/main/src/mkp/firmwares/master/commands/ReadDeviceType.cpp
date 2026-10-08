#include "ReadDeviceType.h"

using namespace mkp::firmwares::master::commands;

ReadDeviceType::ReadDeviceType(
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.type"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceType::~ReadDeviceType() = default;

tfw::utils::void_result ReadDeviceType::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto deviceType = this->deviceConfigurationManager.getDeviceType();
    if (deviceType.empty()) {
        responseWriter.writeLine("0");
    } else {
        responseWriter.writeLine(deviceType);
    }

    return tfw::utils::void_result::success();
}
