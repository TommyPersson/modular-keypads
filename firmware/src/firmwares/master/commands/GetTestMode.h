#pragma once

#include <tfw/utils/commands.h>
#include <firmwares/modules/common/DeviceModule.h>
#include "../TestModeController.h"

class GetTestMode : public tfw::utils::commands::CommandHandler {
public:
    explicit GetTestMode(TestModeController& testModeController);
    ~GetTestMode() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    TestModeController& testModeController;
};
