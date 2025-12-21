#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class SetDeviceAddressCommandHandler final : public CommandHandler {
public:
    explicit SetDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~SetDeviceAddressCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
