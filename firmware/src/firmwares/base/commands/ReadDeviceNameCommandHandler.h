#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "utils/commands/CommandHandler.h"

class ReadDeviceNameCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit ReadDeviceNameCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceNameCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
