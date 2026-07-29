#pragma once

#include <tfw/utils/commands.h>

#include "mkp/devices/common/DeviceConfigurationManager.h"
namespace mkp::firmwares::master::commands {
    class ReadDeviceFirmwareVersion final : public tfw::utils::commands::CommandHandler {
    public:
        explicit ReadDeviceFirmwareVersion(
            mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
        );
        ~ReadDeviceFirmwareVersion() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
    };
}