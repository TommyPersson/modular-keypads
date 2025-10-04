#pragma once

#include <WString.h>

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceAddressCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceAddressCommandHandler() override;
    String execute() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
