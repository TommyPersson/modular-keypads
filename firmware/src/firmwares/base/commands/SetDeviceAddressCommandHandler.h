#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include <tfw/utils/commands.h>

class SetDeviceAddressCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit SetDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~SetDeviceAddressCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
