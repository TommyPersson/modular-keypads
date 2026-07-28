#pragma once

#include <tfw/utils/commands.h>

#include "mkp/devices/common/DeviceConfigurationManager.h"

class ReadDeviceFirmwareVersionCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ReadDeviceFirmwareVersionCommandHandler(
        mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
    );
    ~ReadDeviceFirmwareVersionCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
};
