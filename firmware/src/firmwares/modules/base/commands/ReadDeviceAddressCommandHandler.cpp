#include "ReadDeviceAddressCommandHandler.h"

ReadDeviceAddressCommandHandler::ReadDeviceAddressCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.address", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceAddressCommandHandler::~ReadDeviceAddressCommandHandler() = default;

std::string ReadDeviceAddressCommandHandler::execute(const std::span<const std::string_view>& args) {
    auto address = this->deviceConfigurationManager.getDeviceAddress();
    char addressStr[5];
    snprintf(addressStr, sizeof(addressStr), "0x%02x", address);

    return addressStr;
}
