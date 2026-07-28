#pragma once

#include <tfw/utils/commands.h>

#include "mkp/devices/common/DeviceConfigurationManager.h"

class ResetDeviceCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ResetDeviceCommandHandler(mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager);
    ~ResetDeviceCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
};
