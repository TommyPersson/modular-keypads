#pragma once

#include <tfw/utils/commands.h>
#include "mkp/devices/common/LocalDevice.h"

class ListDeviceCapabilities final : public tfw::utils::commands::CommandHandler {
public:
    explicit ListDeviceCapabilities(std::vector<devices::Device*>& devices);
    ~ListDeviceCapabilities() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    std::vector<devices::Device*>& devices;
};
