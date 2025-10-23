#pragma once

#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/commands/CommandHandler.h"

class ReadDeviceAddressCommandHandler final : public CommandHandler {
public:
    explicit ReadDeviceAddressCommandHandler(DeviceConfigurationManager& deviceConfigurationManager, Logger& logger);
    ~ReadDeviceAddressCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
};
