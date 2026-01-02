#pragma once

#include <utils/commands/CommandHandler.h>
#include <firmwares/modules/common/DeviceModule.h>

class ListConnectedDevices : public CommandHandler {
public:
    explicit ListConnectedDevices(std::vector<devices::DeviceModule*>& devices);
    ~ListConnectedDevices() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    std::vector<devices::DeviceModule*>& devices;
};
