#include "ReadDeviceAddress.h"

#include <tfw/utils/allocations.h>

using namespace mkp::firmwares::master::commands;

ReadDeviceAddress::ReadDeviceAddress(
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
) : CommandHandler("read.device.address"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceAddress::~ReadDeviceAddress() = default;

tfw::utils::void_result ReadDeviceAddress::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    auto address = this->deviceConfigurationManager.getDeviceAddress();
    auto addressStr = tfw::utils::allocations::arena::strings::sprintf2(arena, "0x{:02x}", address);

    responseWriter.writeLine(addressStr);

    return tfw::utils::void_result::success();
}
