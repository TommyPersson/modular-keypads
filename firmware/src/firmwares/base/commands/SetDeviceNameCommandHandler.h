#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include "utils/commands/CommandHandler.h"

class SetDeviceNameCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit SetDeviceNameCommandHandler(std::vector<devices::DeviceModule*>& devices);
    ~SetDeviceNameCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    std::vector<devices::DeviceModule*>& devices;
};
