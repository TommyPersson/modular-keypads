#include "ReadDeviceFirmwareVersionCommandHandler.h"

ReadDeviceFirmwareVersionCommandHandler::ReadDeviceFirmwareVersionCommandHandler(DeviceConfigurationManager& deviceConfigurationManager) :
    CommandHandler("read.device.firmware.version"),
    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersionCommandHandler::~ReadDeviceFirmwareVersionCommandHandler() = default;

std::string ReadDeviceFirmwareVersionCommandHandler::execute() {
    auto deviceId = this->deviceConfigurationManager.getDeviceVersion();
    if (deviceId.empty()) {
        return "NAK";
    }
    return deviceId;
}
