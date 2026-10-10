#include "SetSetting.h"

#include <tfw/utils/strings.h>

#include "mkp/components/settings/Settings.h"

using namespace mkp::components::settings;
using namespace mkp::firmwares::master::commands;

SetSetting::SetSetting(tfw::hal::settings::DeviceSettings& deviceSettings)
    : CommandHandler("set.setting"),
      deviceSettings(deviceSettings) {
}

SetSetting::~SetSetting() = default;

tfw::utils::void_result SetSetting::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (args.size() != 2) {
        return tfw::utils::void_result::error("incorrect.number.of.arguments");
    }

    const auto setting = findUIntSetting(args[0]);
    if (setting == nullptr) {
        return tfw::utils::void_result::error("unknown.setting");
    }

    const auto value = tfw::utils::strings::atou32(args[1]);
    if (!setting->isValid(value)) {
        return tfw::utils::void_result::error("setting.value.out.of.range");
    }

    if (!deviceSettings.putUInt(setting->key, value)) {
        return tfw::utils::void_result::error("unable.to.write.setting");
    }

    return tfw::utils::void_result::success();
}
