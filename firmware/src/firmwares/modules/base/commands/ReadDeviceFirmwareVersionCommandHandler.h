#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceFirmwareVersionCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceFirmwareVersionCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~ReadDeviceFirmwareVersionCommandHandler() override;
    std::string execute(const std::span<const std::string_view>& args, Arena& arena) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
