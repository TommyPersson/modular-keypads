#pragma once

#include <utils/commands/CommandHandler.h>
#include <firmwares/modules/common/DeviceModule.h>
#include "../TestModeController.h"

class SetTestMode : public CommandHandler {
public:
    explicit SetTestMode(TestModeController& testModeController);
    ~SetTestMode() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    TestModeController& testModeController;
};
