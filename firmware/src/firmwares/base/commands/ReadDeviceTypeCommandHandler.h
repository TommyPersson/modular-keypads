#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include <tfw/utils/commands.h>

class ReadDeviceTypeCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ReadDeviceTypeCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceTypeCommandHandler() override;
    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
