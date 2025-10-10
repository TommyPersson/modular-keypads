#include "SetDeviceAddressCommandHandler.h"

#include <utils/strings.h>

SetDeviceAddressCommandHandler::SetDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("set.device.address", logger),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceAddressCommandHandler::~SetDeviceAddressCommandHandler() = default;

std::string SetDeviceAddressCommandHandler::execute(const std::span<const std::string_view>& args) {
    const auto addressStr = args[0];

    const auto address = utils::strings::atol(addressStr, 16);

    if (!this->deviceConfigurationManager.setDeviceAddress(address)) {
        this->logger.error("SetDeviceAddressCommandHandler::execute: unable to set device address");
        return "NAK";
    }

    return "ACK";
}
