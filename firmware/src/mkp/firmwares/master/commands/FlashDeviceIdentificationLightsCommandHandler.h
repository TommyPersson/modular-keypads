#pragma once

#include <tfw/utils/commands.h>

#include "mkp/devices/common/LocalDevice.h"

class FlashDeviceIdentificationLightsCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit FlashDeviceIdentificationLightsCommandHandler(const std::vector<mkp::devices::common::Device*>& devices);
    ~FlashDeviceIdentificationLightsCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    const std::vector<mkp::devices::common::Device*>& devices;
};