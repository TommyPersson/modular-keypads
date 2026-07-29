#pragma once

#include "mkp/devices/common/DeviceConfigurationManager.h"
#include <tfw/utils/commands.h>

namespace mkp::firmwares::master::commands {
    class ReadDeviceType final : public tfw::utils::commands::CommandHandler {
    public:
        explicit ReadDeviceType(mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager);
        ~ReadDeviceType() override;
        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
    };
}