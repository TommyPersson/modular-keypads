#include "ReadDeviceIdCommandHandler.h"


ReadDeviceIdCommandHandler::ReadDeviceIdCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.id", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceIdCommandHandler::~ReadDeviceIdCommandHandler() = default;

std::string ReadDeviceIdCommandHandler::execute(const std::span<const std::string_view>& args) {
    auto deviceId = this->deviceConfigurationManager.getDeviceId();
    if (deviceId.empty()) {
        return "NAK";
    }
    return deviceId;
}
