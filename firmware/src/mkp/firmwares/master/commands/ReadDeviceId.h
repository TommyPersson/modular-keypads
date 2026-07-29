#pragma once

#include "mkp/devices/common/DeviceConfigurationManager.h"
#include <tfw/utils/commands.h>
namespace mkp::firmwares::master::commands {
    class ReadDeviceId final : public tfw::utils::commands::CommandHandler {
    public:
        explicit ReadDeviceId(mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager);
        ~ReadDeviceId() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
    };
}