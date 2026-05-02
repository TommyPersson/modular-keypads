#include "SetTestMode.h"

SetTestMode::SetTestMode(TestModeController& testModeController)
    : CommandHandler("set.test.mode"),
      testModeController(testModeController) {}

SetTestMode::~SetTestMode() = default;

tfw::utils::void_result SetTestMode::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (args.size() != 1) {
        return tfw::utils::void_result::error("incorrect.number.of.arguments");
    }

    const auto& arg = args[0];
    if (arg == "true") {
        testModeController.enable();
    } else {
        testModeController.disable();
    }

    return tfw::utils::void_result::success();
}
