#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "tfw/hal/usb/ModifierHolder.h"
#include "../RecordingHostComputer.h"

using namespace tfw::hal::usb;
using namespace tfw::hal::usb::test;

namespace {
    constexpr uint8_t CONTROL = 0x01;
    constexpr uint8_t SHIFT = 0x02;
    constexpr uint8_t ALT = 0x04;

    using Events = std::vector<std::string>;

    const Events NO_EVENTS{};
}

class ModifierHolderTest : public ::testing::Test {
protected:
    void SetUp() override {
        holder.setDefaultHoldTimeUs(250'000);
    }

    RecordingHostComputer hostComputer;
    ModifierHolder holder{hostComputer};
};

TEST_F(ModifierHolderTest, NothingIsHeldInitially) {
    holder.update(1'000'000);

    EXPECT_EQ(holder.getHeld(), 0);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ModifierHolderTest, HoldPressesModifiers) {
    holder.hold(CONTROL | SHIFT, 0);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"press key 0xe0", "press key 0xe1"}));
    EXPECT_EQ(holder.getHeld(), CONTROL | SHIFT);
}

TEST_F(ModifierHolderTest, HoldingSameModifiersAgainChangesNothing) {
    holder.hold(SHIFT, 0);
    hostComputer.takeEvents();

    holder.hold(SHIFT, 10'000);

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
    EXPECT_EQ(holder.getHeld(), SHIFT);
}

TEST_F(ModifierHolderTest, HoldingOtherModifiersReleasesAndPressesTheDifference) {
    holder.hold(CONTROL | SHIFT, 0);
    hostComputer.takeEvents();

    holder.hold(SHIFT | ALT, 10'000);

    // Shift stays held throughout
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe0", "press key 0xe2"}));
    EXPECT_EQ(holder.getHeld(), SHIFT | ALT);
}

TEST_F(ModifierHolderTest, HoldingNoModifiersReleasesHeldOnes) {
    holder.hold(SHIFT, 0);
    hostComputer.takeEvents();

    holder.hold(0, 10'000);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe1"}));
    EXPECT_EQ(holder.getHeld(), 0);
}

TEST_F(ModifierHolderTest, KeepsModifiersHeldBeforeHoldTimePassed) {
    holder.hold(SHIFT, 0);
    hostComputer.takeEvents();

    holder.update(249'999);

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
    EXPECT_EQ(holder.getHeld(), SHIFT);
}

TEST_F(ModifierHolderTest, ReleasesModifiersWhenHoldTimePassed) {
    holder.hold(SHIFT, 0);
    hostComputer.takeEvents();

    holder.update(250'000);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe1"}));
    EXPECT_EQ(holder.getHeld(), 0);

    holder.update(500'000);

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ModifierHolderTest, EachHoldExtendsHoldTime) {
    // Like fast scrolling, with actions closer together than the hold time
    holder.hold(SHIFT, 0);
    holder.hold(SHIFT, 200'000);
    holder.hold(SHIFT, 400'000);
    holder.update(649'999);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"press key 0xe1"}));

    holder.update(650'000);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe1"}));
}

TEST_F(ModifierHolderTest, ReleaseAllReleasesHeldModifiers) {
    holder.hold(CONTROL | ALT, 0);
    hostComputer.takeEvents();

    holder.releaseAll();

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe0", "release key 0xe2"}));
    EXPECT_EQ(holder.getHeld(), 0);

    holder.releaseAll();

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ModifierHolderTest, ZeroHoldTimeReleasesOnNextUpdate) {
    holder.setDefaultHoldTimeUs(0);
    holder.hold(SHIFT, 1'000);
    hostComputer.takeEvents();

    holder.update(1'000);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe1"}));
}

TEST_F(ModifierHolderTest, HoldWithOwnHoldTimeReleasesAfterIt) {
    holder.hold(ALT, 0, 1'000'000);
    hostComputer.takeEvents();

    holder.update(999'999);

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    holder.update(1'000'000);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe2"}));
}

TEST_F(ModifierHolderTest, LatestHoldDecidesHoldTime) {
    holder.hold(ALT, 0, 1'000'000);
    holder.hold(SHIFT, 100'000);
    hostComputer.takeEvents();

    // The default hold time of the latest hold applies, not the longer one of the earlier hold
    holder.update(350'000);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe1"}));
}

TEST_F(ModifierHolderTest, ChangingDefaultHoldTimeKeepsOwnHoldTime) {
    holder.hold(ALT, 0, 1'000'000);
    hostComputer.takeEvents();

    holder.setDefaultHoldTimeUs(100'000);
    holder.update(500'000);

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}
