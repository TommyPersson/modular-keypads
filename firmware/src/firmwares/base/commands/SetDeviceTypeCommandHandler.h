#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class SetDeviceTypeCommandHandler final : public CommandHandler {
public:
    explicit SetDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~SetDeviceTypeCommandHandler() override;
    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
