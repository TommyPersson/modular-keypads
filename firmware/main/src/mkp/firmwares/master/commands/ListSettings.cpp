#include "ListSettings.h"

#include "mkp/components/settings/Settings.h"

using namespace mkp::components::settings;
using namespace mkp::firmwares::master::commands;

ListSettings::ListSettings(tfw::hal::settings::DeviceSettings& deviceSettings)
    : CommandHandler("list.settings"),
      deviceSettings(deviceSettings) {
}

ListSettings::~ListSettings() = default;

tfw::utils::void_result ListSettings::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    for (const auto& setting : AllUIntSettings) {
        responseWriter.writeLineF(
            "%s:uint:%lu:%lu:%lu:%lu",
            setting.key,
            static_cast<unsigned long>(setting.get(deviceSettings)),
            static_cast<unsigned long>(setting.defaultValue),
            static_cast<unsigned long>(setting.minValue),
            static_cast<unsigned long>(setting.maxValue)
        );
    }

    return tfw::utils::void_result::success();
}
