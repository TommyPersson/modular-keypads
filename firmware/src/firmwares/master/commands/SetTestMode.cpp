#include "SetTestMode.h"

SetTestMode::SetTestMode(TestModeController& testModeController)
    : CommandHandler("set.test.mode"),
      testModeController(testModeController) {}

SetTestMode::~SetTestMode() = default;

void SetTestMode::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    if (args.size() != 1) {
        responseWriter.writeLine("NAK");
        return;
    }

    const auto& arg = args[0];
    if (arg == "true") {
        testModeController.enable();
    } else {
        testModeController.disable();
    }
}
