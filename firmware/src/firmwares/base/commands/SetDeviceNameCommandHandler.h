#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "utils/commands/CommandHandler.h"

class SetDeviceNameCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit SetDeviceNameCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~SetDeviceNameCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
