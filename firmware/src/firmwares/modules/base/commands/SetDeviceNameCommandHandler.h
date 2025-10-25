#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class SetDeviceNameCommandHandler final : public CommandHandler {
public:
    explicit SetDeviceNameCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~SetDeviceNameCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
