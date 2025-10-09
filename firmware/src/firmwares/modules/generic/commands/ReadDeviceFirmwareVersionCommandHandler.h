#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceFirmwareVersionCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceFirmwareVersionCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceFirmwareVersionCommandHandler() override;
    std::string execute() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
