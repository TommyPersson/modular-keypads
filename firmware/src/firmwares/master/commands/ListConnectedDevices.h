#pragma once

#include <firmwares/common/commands/CommandHandler.h>
#include <firmwares/modules/common/DeviceModule.h>

class ListConnectedDevices : public CommandHandler {
public:
    explicit ListConnectedDevices(std::vector<std::unique_ptr<devices::DeviceModule>>& devices);
    ~ListConnectedDevices() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    std::vector<std::unique_ptr<devices::DeviceModule>>& devices;
};
