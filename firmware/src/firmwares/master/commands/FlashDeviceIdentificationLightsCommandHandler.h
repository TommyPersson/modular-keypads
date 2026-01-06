#pragma once

#include "firmwares/modules/common/DeviceModule.h"
#include "utils/commands/CommandHandler.h"

class FlashDeviceIdentificationLightsCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit FlashDeviceIdentificationLightsCommandHandler(const std::vector<devices::DeviceModule*>& devices);
    ~FlashDeviceIdentificationLightsCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    const std::vector<devices::DeviceModule*>& devices;
};