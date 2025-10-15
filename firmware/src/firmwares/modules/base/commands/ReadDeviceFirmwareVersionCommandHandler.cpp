#include "ReadDeviceFirmwareVersionCommandHandler.h"

ReadDeviceFirmwareVersionCommandHandler::ReadDeviceFirmwareVersionCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.firmware.version", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersionCommandHandler::~ReadDeviceFirmwareVersionCommandHandler() = default;

std::string ReadDeviceFirmwareVersionCommandHandler::execute(const std::span<const std::string_view>& args, Arena& arena) {
    auto deviceId = this->deviceConfigurationManager.getDeviceVersion();
    if (deviceId.empty()) {
        return "NAK";
    }
    return deviceId;
}
