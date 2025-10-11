#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ResetDeviceCommandHandler final : public CommandHandler {
public:
    explicit ResetDeviceCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~ResetDeviceCommandHandler() override;
    std::string execute(const std::span<const std::string_view>& args) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
