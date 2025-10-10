#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceTypeCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~ReadDeviceTypeCommandHandler() override;
    std::string execute(const std::span<const std::string_view>& args) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
