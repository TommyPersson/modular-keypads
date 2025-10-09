 #include "ReadDeviceAddressCommandHandler.h"

ReadDeviceAddressCommandHandler::ReadDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.address"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceAddressCommandHandler::~ReadDeviceAddressCommandHandler() = default;

std::string ReadDeviceAddressCommandHandler::execute() {
    auto address = this->deviceConfigurationManager.getDeviceAddress();
    char addressStr[5];
    snprintf(addressStr, sizeof(addressStr), "%02x", address);

    return addressStr;
}
