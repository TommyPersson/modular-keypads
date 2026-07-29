#pragma once

#include <tfw/utils/commands.h>

#include "mkp/devices/common/LocalDevice.h"

namespace mkp::firmwares::master::commands {
    class SetDeviceName final : public tfw::utils::commands::CommandHandler {
    public:
        explicit SetDeviceName(std::vector<mkp::devices::common::Device*>& devices);
        ~SetDeviceName() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        std::vector<mkp::devices::common::Device*>& devices;
    };
}