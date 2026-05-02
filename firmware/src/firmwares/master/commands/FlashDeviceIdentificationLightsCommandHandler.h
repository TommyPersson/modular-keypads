#pragma once

#include "firmwares/modules/common/DeviceModule.h"
#include <tfw/utils/commands.h>

class FlashDeviceIdentificationLightsCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit FlashDeviceIdentificationLightsCommandHandler(const std::vector<devices::DeviceModule*>& devices);
    ~FlashDeviceIdentificationLightsCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    const std::vector<devices::DeviceModule*>& devices;
};