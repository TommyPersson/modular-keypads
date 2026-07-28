#pragma once

#include <tfw/utils/commands.h>
#include "mkp/devices/common/LocalDevice.h"

class ListConnectedDevices : public tfw::utils::commands::CommandHandler {
public:
    explicit ListConnectedDevices(std::vector<devices::Device*>& devices);
    ~ListConnectedDevices() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    std::vector<devices::Device*>& devices;
};
