#include <gtest/gtest.h>
#include <memory>

#include "mkp/components/keybindings/MacroCompiler.h"
#include "mkp/components/keybindings/MacroCompiler.cpp"

using namespace mkp::components::keybindings;
using namespace mkp::components::macros;
using tfw::hal::usb::MidiControlChangeAction;

namespace {
    Macro createRelativeControlChangeMacro(
        const int8_t amount,
        const RelativeControlChangeEncoding encoding,
        const uint8_t channel = 2,
        const uint8_t controller = 7
    ) {
        auto data = std::make_shared<MidiRelativeControlChangeMacroData>(1, channel, controller, amount, encoding);
        return Macro{"Volume", data};
    }

    const MidiControlChangeAction& onlyControlChange(const CompiledMacro& macro) {
        EXPECT_EQ(macro.actions.size(), 1);
        return dynamic_cast<const MidiControlChangeAction&>(*macro.actions[0]);
    }
}

// ============================================================================
// Relative control change
// ============================================================================

TEST(MacroCompilerTest, RelativeControlChangeSendsChannelAndController) {
    const auto compiled = compileMacro(createRelativeControlChangeMacro(1, RelativeControlChangeEncoding::BINARY_OFFSET));

    ASSERT_NE(compiled, nullptr);
    const auto& action = onlyControlChange(*compiled);
    EXPECT_EQ(action.channel, 2);
    EXPECT_EQ(action.controller, 7);
    EXPECT_TRUE(compiled->releaseActions.empty());
}

TEST(MacroCompilerTest, BinaryOffsetEncodingIsOffsetFrom64) {
    constexpr auto encoding = RelativeControlChangeEncoding::BINARY_OFFSET;

    EXPECT_EQ(encodeRelativeControlChange(1, encoding), 65);
    EXPECT_EQ(encodeRelativeControlChange(-1, encoding), 63);
    EXPECT_EQ(encodeRelativeControlChange(63, encoding), 127);
    EXPECT_EQ(encodeRelativeControlChange(-63, encoding), 1);
}

TEST(MacroCompilerTest, TwosComplementEncodingWrapsNegativeAmounts) {
    constexpr auto encoding = RelativeControlChangeEncoding::TWOS_COMPLEMENT;

    EXPECT_EQ(encodeRelativeControlChange(1, encoding), 1);
    EXPECT_EQ(encodeRelativeControlChange(-1, encoding), 127);
    EXPECT_EQ(encodeRelativeControlChange(63, encoding), 63);
    EXPECT_EQ(encodeRelativeControlChange(-63, encoding), 65);
}

TEST(MacroCompilerTest, SignMagnitudeEncodingSetsBit6ForNegativeAmounts) {
    constexpr auto encoding = RelativeControlChangeEncoding::SIGN_MAGNITUDE;

    EXPECT_EQ(encodeRelativeControlChange(1, encoding), 1);
    EXPECT_EQ(encodeRelativeControlChange(-1, encoding), 65);
    EXPECT_EQ(encodeRelativeControlChange(63, encoding), 63);
    EXPECT_EQ(encodeRelativeControlChange(-63, encoding), 127);
}

TEST(MacroCompilerTest, RelativeControlChangeAmountBeyondEncodingRangeIsLimited) {
    EXPECT_EQ(encodeRelativeControlChange(100, RelativeControlChangeEncoding::BINARY_OFFSET), 127);
    EXPECT_EQ(encodeRelativeControlChange(-100, RelativeControlChangeEncoding::TWOS_COMPLEMENT), 65);
}

TEST(MacroCompilerTest, RelativeControlChangeMacroUsesItsEncoding) {
    const auto compiled = compileMacro(createRelativeControlChangeMacro(-2, RelativeControlChangeEncoding::TWOS_COMPLEMENT));

    ASSERT_NE(compiled, nullptr);
    EXPECT_EQ(onlyControlChange(*compiled).value, 126);
}
