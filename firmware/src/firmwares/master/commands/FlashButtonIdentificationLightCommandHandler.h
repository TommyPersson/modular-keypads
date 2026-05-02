#pragma once

#include "firmwares/modules/common/DeviceModule.h"
#include <tfw/utils/commands.h>

class FlashButtonIdentificationLightCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit FlashButtonIdentificationLightCommandHandler(const std::vector<devices::DeviceModule*>& devices);
    ~FlashButtonIdentificationLightCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    const std::vector<devices::DeviceModule*>& devices;
};