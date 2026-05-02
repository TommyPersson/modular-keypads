#include "GetTestMode.h"

GetTestMode::GetTestMode(TestModeController& testModeController)
    : CommandHandler("get.test.mode"),
      testModeController(testModeController) {
}

GetTestMode::~GetTestMode() = default;

tfw::utils::void_result GetTestMode::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    responseWriter.writeLine(testModeController.isEnabled() ? "true" : "false");

    return tfw::utils::void_result::success();
}
