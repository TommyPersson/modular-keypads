#include "GetTestMode.h"

GetTestMode::GetTestMode(TestModeController& testModeController)
    : CommandHandler("get.test.mode"),
      testModeController(testModeController) {
}

GetTestMode::~GetTestMode() = default;

utils::void_result GetTestMode::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    responseWriter.writeLine(testModeController.isEnabled() ? "true" : "false");

    return utils::void_result::success();
}
