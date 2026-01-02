#pragma once

#include <utils/commands/CommandHandler.h>
#include <firmwares/modules/common/DeviceModule.h>

class ListDeviceCapabilities final : public CommandHandler {
public:
    explicit ListDeviceCapabilities(std::vector<devices::DeviceModule*>& devices);
    ~ListDeviceCapabilities() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    std::vector<devices::DeviceModule*>& devices;
};
