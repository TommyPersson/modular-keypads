#pragma once

#include "mkp/devices/common/DeviceConfigurationManager.h"
#include <tfw/utils/commands.h>

class ReadDeviceNameCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ReadDeviceNameCommandHandler(mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceNameCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
};
