#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include <tfw/utils/commands.h>

class SetDeviceNameCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit SetDeviceNameCommandHandler(std::vector<devices::DeviceModule*>& devices);
    ~SetDeviceNameCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    std::vector<devices::DeviceModule*>& devices;
};
