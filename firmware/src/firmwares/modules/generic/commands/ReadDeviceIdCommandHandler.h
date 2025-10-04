#pragma once

#include <WString.h>

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceIdCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceIdCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceIdCommandHandler() override;
    String execute() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
