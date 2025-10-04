#include "ReadDeviceFirmwareVersionCommandHandler.h"

ReadDeviceFirmwareVersionCommandHandler::ReadDeviceFirmwareVersionCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.firmware.version"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersionCommandHandler::~ReadDeviceFirmwareVersionCommandHandler() = default;

String ReadDeviceFirmwareVersionCommandHandler::execute() {
    auto deviceId = this->deviceConfigurationManager.getDeviceVersion();
    if (deviceId.isEmpty()) {
        return "NAK";
    }
    return deviceId;
}
