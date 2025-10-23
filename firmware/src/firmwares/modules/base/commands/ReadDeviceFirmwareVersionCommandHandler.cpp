#include "ReadDeviceFirmwareVersionCommandHandler.h"

ReadDeviceFirmwareVersionCommandHandler::ReadDeviceFirmwareVersionCommandHandler(
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) :
    CommandHandler("read.device.firmware.version", logger),

    deviceConfigurationManager(deviceConfigurationManager) {
}

ReadDeviceFirmwareVersionCommandHandler::~ReadDeviceFirmwareVersionCommandHandler() = default;

void ReadDeviceFirmwareVersionCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto deviceVersion = this->deviceConfigurationManager.getDeviceVersion();
    if (deviceVersion.empty()) {
        responseWriter.writeLine("NAK");
        return;
    }

    responseWriter.writeLine(deviceVersion);
}
