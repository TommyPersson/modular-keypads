#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class SetDeviceAddressCommandHandler final : public CommandHandler {
public:
    explicit SetDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~SetDeviceAddressCommandHandler() override;
    std::string execute(const std::span<const std::string_view>& args, Arena& arena) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
