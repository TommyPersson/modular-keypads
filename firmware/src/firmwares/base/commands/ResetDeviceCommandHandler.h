#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "utils/commands/CommandHandler.h"

class ResetDeviceCommandHandler final : public CommandHandler {
public:
    explicit ResetDeviceCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ResetDeviceCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
