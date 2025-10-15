#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class SetDeviceTypeCommandHandler final : public CommandHandler {
public:
    explicit SetDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~SetDeviceTypeCommandHandler() override;
    std::string execute(const std::span<const std::string_view>& args, Arena& arena) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
