#include "ReadDeviceTypeCommandHandler.h"

ReadDeviceTypeCommandHandler::ReadDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.type"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceTypeCommandHandler::~ReadDeviceTypeCommandHandler() = default;

String ReadDeviceTypeCommandHandler::execute() {
    auto deviceId = this->deviceConfigurationManager.getDeviceType();
    if (deviceId == 0) {
        return "generic";
    }
    return String(deviceId); // TODO stack string
}
