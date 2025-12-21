#include "GetTestMode.h"

GetTestMode::GetTestMode(TestModeController& testModeController)
    : CommandHandler("get.test.mode"),
      testModeController(testModeController) {}

GetTestMode::~GetTestMode() = default;

void GetTestMode::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    responseWriter.writeLine(testModeController.isEnabled() ? "true" : "false");
}
