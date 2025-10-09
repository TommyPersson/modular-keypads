#include "ReadDeviceTypeCommandHandler.h"

ReadDeviceTypeCommandHandler::ReadDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.type"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceTypeCommandHandler::~ReadDeviceTypeCommandHandler() = default;

std::string ReadDeviceTypeCommandHandler::execute() {
    auto deviceId = this->deviceConfigurationManager.getDeviceType();
    if (deviceId == 0) {
        return "generic";
    }
    return std::string(1, deviceId);
}
