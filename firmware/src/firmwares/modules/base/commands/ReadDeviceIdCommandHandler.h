#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceIdCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceIdCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~ReadDeviceIdCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
