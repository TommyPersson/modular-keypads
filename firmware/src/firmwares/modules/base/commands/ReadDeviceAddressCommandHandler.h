#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceAddressCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~ReadDeviceAddressCommandHandler() override;
    std::string execute(const std::span<const std::string_view>& args) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
