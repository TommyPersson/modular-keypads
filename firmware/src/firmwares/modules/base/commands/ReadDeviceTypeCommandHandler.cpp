#include "ReadDeviceTypeCommandHandler.h"

ReadDeviceTypeCommandHandler::ReadDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.type", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceTypeCommandHandler::~ReadDeviceTypeCommandHandler() = default;

std::string ReadDeviceTypeCommandHandler::execute(const std::span<const std::string_view>& args) {
    auto deviceId = this->deviceConfigurationManager.getDeviceType();
    if (deviceId == 0) {
        return "0";
    }

    return {&deviceId, 1};
}
