#include "ReadDeviceIdCommandHandler.h"


ReadDeviceIdCommandHandler::ReadDeviceIdCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.id"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceIdCommandHandler::~ReadDeviceIdCommandHandler() = default;

std::string ReadDeviceIdCommandHandler::execute() {
    auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId.empty()) {
        return "NAK";
    }
    return deviceId;
}
