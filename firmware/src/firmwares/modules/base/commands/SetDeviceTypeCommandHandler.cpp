#include "SetDeviceTypeCommandHandler.h"

SetDeviceTypeCommandHandler::SetDeviceTypeCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("set.device.type", logger),
    deviceConfigurationManager(deviceConfigurationManager) {
}

SetDeviceTypeCommandHandler::~SetDeviceTypeCommandHandler() = default;

std::string SetDeviceTypeCommandHandler::execute(const std::span<const std::string_view>& args, Arena& arena) {
    if (args.size() != 1) {
        this->logger.error("SetDeviceTypeCommandHandler::execute: wrong number of arguments");
        return "NAK";
    }

    if (args[0].size() != 1) {
        this->logger.error("SetDeviceTypeCommandHandler::execute: wrong number of characters for device type");
        return "NAK";
    }

    const auto deviceTypeCode = args[0][0];

    if (!deviceConfigurationManager.setDeviceType(deviceTypeCode)) {
        this->logger.error("SetDeviceTypeCommandHandler::execute: unable to set device type");
        return "NAK";
    }

    return "ACK";
}
