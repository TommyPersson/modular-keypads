#include "ResetDevice.h"

using namespace mkp::firmwares::master::commands;

ResetDevice::ResetDevice(
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("reset.device"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ResetDevice::~ResetDevice() = default;

tfw::utils::void_result ResetDevice::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    ESP.restart();

    return tfw::utils::void_result::success();
}
