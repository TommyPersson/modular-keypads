#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceNameCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceNameCommandHandler(DeviceConfigurationManager& deviceConfigurationManager);
    ~ReadDeviceNameCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
