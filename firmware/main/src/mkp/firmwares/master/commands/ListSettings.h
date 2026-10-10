#pragma once

#include <tfw/hal/settings.h>
#include <tfw/utils/commands.h>

namespace mkp::firmwares::master::commands {
    class ListSettings : public tfw::utils::commands::CommandHandler {
    public:
        explicit ListSettings(tfw::hal::settings::DeviceSettings& deviceSettings);
        ~ListSettings() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        tfw::hal::settings::DeviceSettings& deviceSettings;
    };
}
