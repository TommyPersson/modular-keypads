#include "SetTestMode.h"

SetTestMode::SetTestMode(TestModeController& testModeController)
    : CommandHandler("set.test.mode"),
      testModeController(testModeController) {}

SetTestMode::~SetTestMode() = default;

utils::void_result SetTestMode::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    if (args.size() != 1) {
        return utils::void_result::error("incorrect.number.of.arguments");
    }

    const auto& arg = args[0];
    if (arg == "true") {
        testModeController.enable();
    } else {
        testModeController.disable();
    }

    return utils::void_result::success();
}
