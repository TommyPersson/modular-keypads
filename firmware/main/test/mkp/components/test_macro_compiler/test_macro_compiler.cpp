#include <gtest/gtest.h>
#include <memory>

#include "mkp/components/keybindings/MacroCompiler.h"
#include "mkp/components/keybindings/MacroCompiler.cpp"
#include "mkp/components/generators/RandomTextGenerator.cpp"
#include "mkp/components/generators/UuidV4Generator.cpp"
#include "../../../lib/tfw/hal/StubRandomSource.h"

using namespace mkp::components::keybindings;
using namespace mkp::components::macros;
using tfw::hal::usb::MidiControlChangeAction;
using tfw::hal::random::test::StubRandomSource;

namespace {
    StubRandomSource randomSource{{0x00}};

    std::shared_ptr<CompiledMacro> compileMacro(const Macro& macro) {
        return mkp::components::keybindings::compileMacro(macro, randomSource);
    }

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

// ============================================================================
// Absolute control change
// ============================================================================

TEST(MacroCompilerTest, AbsoluteControlChangeStepsTheController) {
    const auto data = std::make_shared<MidiAbsoluteControlChangeMacroData>(1, 3, 7, -2);
    const auto compiled = compileMacro(Macro{"Volume Down", data});

    ASSERT_NE(compiled, nullptr);
    ASSERT_EQ(compiled->actions.size(), 1);
    const auto& action = dynamic_cast<const tfw::hal::usb::MidiControlChangeStepAction&>(*compiled->actions[0]);
    EXPECT_EQ(action.channel, 3);
    EXPECT_EQ(action.controller, 7);
    EXPECT_EQ(action.amount, -2);
}

// ============================================================================
// Random text
// ============================================================================

TEST(MacroCompilerTest, RandomTextIsGeneratedEachTimeTheActionIsSent) {
    StubRandomSource source{{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    }};
    tfw::utils::allocations::Arena arena{128};
    const auto data = std::make_shared<RandomTextMacroData>(1, RandomTextFormat::UUID_V4);
    const auto compiled = mkp::components::keybindings::compileMacro(Macro{"UUID", data}, source);

    ASSERT_NE(compiled, nullptr);
    ASSERT_EQ(compiled->actions.size(), 1);
    const auto& action = dynamic_cast<const tfw::hal::usb::GeneratedTypeAction&>(*compiled->actions[0]);
    EXPECT_EQ(action.generate(arena), "00000000-0000-4000-8000-000000000000");
    EXPECT_EQ(action.generate(arena), "01010101-0101-4101-8101-010101010101");
}

TEST(MacroCompilerTest, QuotedRandomTextIsWrappedInDoubleQuotes) {
    StubRandomSource source{{0x00}};
    tfw::utils::allocations::Arena arena{64};
    const auto data = std::make_shared<RandomTextMacroData>(1, RandomTextFormat::UUID_V4, true);
    const auto compiled = mkp::components::keybindings::compileMacro(Macro{"UUID", data}, source);

    ASSERT_NE(compiled, nullptr);
    const auto& action = dynamic_cast<const tfw::hal::usb::GeneratedTypeAction&>(*compiled->actions[0]);
    EXPECT_EQ(action.generate(arena), "\"00000000-0000-4000-8000-000000000000\"");
}

TEST(MacroCompilerTest, RandomTextWithUnknownFormatIsNotCompiled) {
    const auto data = std::make_shared<RandomTextMacroData>(1, static_cast<RandomTextFormat>(99));

    EXPECT_EQ(compileMacro(Macro{"Later", data}), nullptr);
}
