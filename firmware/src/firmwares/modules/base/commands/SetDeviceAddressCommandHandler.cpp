#include "SetDeviceAddressCommandHandler.h"

#include <utils/strings.h>

namespace {
    auto logger = common::logging::createLogger("SetDeviceAddressCommandHandler");
}

SetDeviceAddressCommandHandler::SetDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager
    ) :
    CommandHandler("set.device.address"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceAddressCommandHandler::~SetDeviceAddressCommandHandler() = default;

void SetDeviceAddressCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto addressStr = args[0];

    const auto address = utils::strings::atol(addressStr, 16);

    if (!this->deviceConfigurationManager.setDeviceAddress(address)) {
        logger->error("execute: unable to set device address");
        responseWriter.writeLine("NAK");
    }
}
