#pragma once

#include "../../../mkp/devices/common/DeviceConfigurationManager.h"
#include <tfw/utils/commands.h>

class ReadDeviceAddressCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ReadDeviceAddressCommandHandler(
        mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager
    );
    ~ReadDeviceAddressCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
};
