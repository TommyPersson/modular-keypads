#include <gtest/gtest.h>
#include "tfw/hal/usb/ModifierHolder.h"

using namespace tfw::hal::usb;

namespace {
    constexpr uint8_t CONTROL = 0x01;
    constexpr uint8_t SHIFT = 0x02;
    constexpr uint8_t ALT = 0x04;
}

class ModifierHolderTest : public ::testing::Test {
protected:
    void SetUp() override {
        holder.setDefaultHoldTimeUs(250'000);
    }

    ModifierHolder holder;
};

TEST_F(ModifierHolderTest, NothingIsHeldInitially) {
    EXPECT_EQ(holder.getHeld(), 0);
    EXPECT_EQ(holder.update(1'000'000), ModifierChange{});
}

TEST_F(ModifierHolderTest, HoldPressesModifiers) {
    EXPECT_EQ(holder.hold(CONTROL | SHIFT, 0), (ModifierChange{.press = CONTROL | SHIFT}));
    EXPECT_EQ(holder.getHeld(), CONTROL | SHIFT);
}

TEST_F(ModifierHolderTest, HoldingSameModifiersAgainChangesNothing) {
    holder.hold(SHIFT, 0);

    EXPECT_EQ(holder.hold(SHIFT, 10'000), ModifierChange{});
    EXPECT_EQ(holder.getHeld(), SHIFT);
}

TEST_F(ModifierHolderTest, HoldingOtherModifiersPressesAndReleasesTheDifference) {
    holder.hold(CONTROL | SHIFT, 0);

    EXPECT_EQ(holder.hold(SHIFT | ALT, 10'000), (ModifierChange{.press = ALT, .release = CONTROL}));
    EXPECT_EQ(holder.getHeld(), SHIFT | ALT);
}

TEST_F(ModifierHolderTest, HoldingNoModifiersReleasesHeldOnes) {
    holder.hold(SHIFT, 0);

    EXPECT_EQ(holder.hold(0, 10'000), (ModifierChange{.release = SHIFT}));
    EXPECT_EQ(holder.getHeld(), 0);
}

TEST_F(ModifierHolderTest, KeepsModifiersHeldBeforeHoldTimePassed) {
    holder.hold(SHIFT, 0);

    EXPECT_EQ(holder.update(249'999), ModifierChange{});
    EXPECT_EQ(holder.getHeld(), SHIFT);
}

TEST_F(ModifierHolderTest, ReleasesModifiersWhenHoldTimePassed) {
    holder.hold(SHIFT, 0);

    EXPECT_EQ(holder.update(250'000), (ModifierChange{.release = SHIFT}));
    EXPECT_EQ(holder.getHeld(), 0);
    EXPECT_EQ(holder.update(500'000), ModifierChange{});
}

TEST_F(ModifierHolderTest, EachHoldExtendsHoldTime) {
    // Like fast scrolling, with actions closer together than the hold time
    holder.hold(SHIFT, 0);
    holder.hold(SHIFT, 200'000);
    holder.hold(SHIFT, 400'000);

    EXPECT_EQ(holder.update(649'999), ModifierChange{});
    EXPECT_EQ(holder.update(650'000), (ModifierChange{.release = SHIFT}));
}

TEST_F(ModifierHolderTest, ReleaseAllReleasesHeldModifiers) {
    holder.hold(CONTROL | ALT, 0);

    EXPECT_EQ(holder.releaseAll(), (ModifierChange{.release = CONTROL | ALT}));
    EXPECT_EQ(holder.getHeld(), 0);
    EXPECT_EQ(holder.releaseAll(), ModifierChange{});
}

TEST_F(ModifierHolderTest, ZeroHoldTimeReleasesOnNextUpdate) {
    holder.setDefaultHoldTimeUs(0);
    holder.hold(SHIFT, 1'000);

    EXPECT_EQ(holder.update(1'000), (ModifierChange{.release = SHIFT}));
}

TEST_F(ModifierHolderTest, HoldWithOwnHoldTimeReleasesAfterIt) {
    holder.hold(ALT, 0, 1'000'000);

    EXPECT_EQ(holder.update(999'999), ModifierChange{});
    EXPECT_EQ(holder.update(1'000'000), (ModifierChange{.release = ALT}));
}

TEST_F(ModifierHolderTest, LatestHoldDecidesHoldTime) {
    holder.hold(ALT, 0, 1'000'000);
    holder.hold(SHIFT, 100'000);

    // The default hold time of the latest hold applies, not the longer one of the earlier hold
    EXPECT_EQ(holder.update(350'000), (ModifierChange{.release = SHIFT}));
}

TEST_F(ModifierHolderTest, ChangingDefaultHoldTimeKeepsOwnHoldTime) {
    holder.hold(ALT, 0, 1'000'000);
    holder.setDefaultHoldTimeUs(100'000);

    EXPECT_EQ(holder.update(500'000), ModifierChange{});
}
