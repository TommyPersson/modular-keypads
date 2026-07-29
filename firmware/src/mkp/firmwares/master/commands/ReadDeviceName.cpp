#include "ReadDeviceName.h"


using namespace mkp::firmwares::master::commands;

ReadDeviceName::ReadDeviceName(
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.name"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceName::~ReadDeviceName() = default;

tfw::utils::void_result ReadDeviceName::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    const auto name = this->deviceConfigurationManager.getDeviceName();

    responseWriter.writeLine(name);

    return tfw::utils::void_result::success();
}
