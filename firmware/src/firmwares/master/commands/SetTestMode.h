#pragma once

#include <firmwares/common/commands/CommandHandler.h>
#include <firmwares/modules/common/DeviceModule.h>
#include "../TestModeController.h"

class SetTestMode : public CommandHandler {
public:
    explicit SetTestMode(TestModeController& testModeController);
    ~SetTestMode() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    TestModeController& testModeController;
};
