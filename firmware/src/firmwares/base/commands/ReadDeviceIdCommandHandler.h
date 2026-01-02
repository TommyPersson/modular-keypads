#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "utils/commands/CommandHandler.h"

class ReadDeviceIdCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit ReadDeviceIdCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceIdCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
