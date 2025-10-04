#include "ReadDeviceIdCommandHandler.h"


ReadDeviceIdCommandHandler::ReadDeviceIdCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.id"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceIdCommandHandler::~ReadDeviceIdCommandHandler() = default;

String ReadDeviceIdCommandHandler::execute() {
    auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId.isEmpty()) {
        return "NAK";
    }
    return deviceId;
}
