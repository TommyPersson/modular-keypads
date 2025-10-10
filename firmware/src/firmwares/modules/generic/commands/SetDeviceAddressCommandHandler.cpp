#include "SetDeviceAddressCommandHandler.h"

SetDeviceAddressCommandHandler::SetDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("set.device.address", logger),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceAddressCommandHandler::~SetDeviceAddressCommandHandler() = default;

std::string SetDeviceAddressCommandHandler::execute(const std::span<const std::string_view>& args) {
    this->logger.info("TODO");
    return "ACK";
}
