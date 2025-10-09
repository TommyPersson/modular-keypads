#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceTypeCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceTypeCommandHandler() override;
    std::string execute() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
