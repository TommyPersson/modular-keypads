#pragma once

#include <tfw/hal/settings.h>
#include <tfw/utils/commands.h>

namespace mkp::firmwares::master::commands {
    class SetSetting : public tfw::utils::commands::CommandHandler {
    public:
        explicit SetSetting(tfw::hal::settings::DeviceSettings& deviceSettings);
        ~SetSetting() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        tfw::hal::settings::DeviceSettings& deviceSettings;
    };
}
