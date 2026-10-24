#pragma once

#include <tfw/utils/commands.h>

#include "mkp/components/firmware/FirmwareUpdater.h"

namespace mkp::firmwares::master::commands {
    class BeginFirmwareUpdate final : public tfw::utils::commands::CommandHandler {
    public:
        explicit BeginFirmwareUpdate(mkp::components::firmware::FirmwareUpdater& firmwareUpdater);
        ~BeginFirmwareUpdate() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        mkp::components::firmware::FirmwareUpdater& firmwareUpdater;
    };
}
