#pragma once

#include <WString.h>

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceFirmwareVersionCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceFirmwareVersionCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceFirmwareVersionCommandHandler() override;
    String execute() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
