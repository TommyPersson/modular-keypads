#pragma once

#include <tfw/utils/commands.h>

#include "mkp/devices/common/LocalDevice.h"

namespace mkp::firmwares::master::commands {
    class FlashDeviceIdentificationLights final : public tfw::utils::commands::CommandHandler {
    public:
        explicit FlashDeviceIdentificationLights(const std::vector<mkp::devices::common::Device*>& devices);
        ~FlashDeviceIdentificationLights() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        const std::vector<mkp::devices::common::Device*>& devices;
    };
}