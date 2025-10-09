#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceIdCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceIdCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceIdCommandHandler() override;
    std::string execute() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
