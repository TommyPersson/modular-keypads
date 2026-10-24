#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "tfw/hal/usb/ActionSender.h"
#include "../RecordingHostComputer.h"
#include "../VirtualClock.h"

using namespace tfw::hal::usb;
using namespace tfw::hal::usb::test;
using namespace tfw::hal::time::test;

namespace {
    constexpr uint8_t CONTROL = 0x01;
    constexpr uint8_t SHIFT = 0x02;
    constexpr uint8_t ALT = 0x04;

    constexpr uint8_t KEY_C = 0x06;
    constexpr uint8_t KEY_TAB = 0x2b;

    using Events = std::vector<std::string>;

    const Events NO_EVENTS{};
}

class ActionSenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Like a device that has been running for a while
        clock.setTimeUs(10'000'000);
        sender.setTypingDelayMs(10);
        sender.setModifierHoldMs(250);
    }

    void send(const std::shared_ptr<Action>& action) {
        sender.send(*action);
    }

    // Advances time in steps, updating the sender after each step, like the main loop does
    void runForMs(const uint32_t durationMs) {
        for (uint32_t elapsedMs = 0; elapsedMs < durationMs; elapsedMs++) {
            clock.advanceMs(1);
            sender.update();
        }
    }

    VirtualClock clock;
    RecordingHostComputer hostComputer;
    ActionSender sender{hostComputer, clock};
};

// ============================================================================
// Shortcuts
// ============================================================================

TEST_F(ActionSenderTest, ShortcutPressesKeyWithModifiersAndReleasesAll) {
    send(Action::shortcut(CONTROL, KEY_C, false, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe0",
        "press key 0x06",
        "release key 0x06",
        "release key 0xe0",
    }));
}

TEST_F(ActionSenderTest, ShortcutHoldingModifiersKeepsThemHeldBetweenRepeats) {
    send(Action::shortcut(ALT, KEY_TAB, true, 0));
    runForMs(100);
    send(Action::shortcut(ALT, KEY_TAB, true, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe2",
        "press key 0x2b",
        "release key 0x2b",
        "press key 0x2b",
        "release key 0x2b",
    }));

    runForMs(249);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    runForMs(1);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe2"}));
}

TEST_F(ActionSenderTest, ShortcutHoldingModifiersUsesOwnHoldTime) {
    send(Action::shortcut(ALT, KEY_TAB, true, 800));
    hostComputer.takeEvents();

    runForMs(799);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    runForMs(1);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe2"}));
}

TEST_F(ActionSenderTest, ChangingDirectionOfHeldShortcutKeepsSharedModifierHeld) {
    // Like turning a rotary encoder back and forth through the window switcher
    send(Action::shortcut(ALT, KEY_TAB, true, 0));
    send(Action::shortcut(ALT | SHIFT, KEY_TAB, true, 0));
    send(Action::shortcut(ALT, KEY_TAB, true, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe2",
        "press key 0x2b",
        "release key 0x2b",
        "press key 0xe1",
        "press key 0x2b",
        "release key 0x2b",
        "release key 0xe1",
        "press key 0x2b",
        "release key 0x2b",
    }));
}

TEST_F(ActionSenderTest, ShortcutWithoutHoldReleasesModifiersHeldBefore) {
    send(Action::shortcut(ALT, KEY_TAB, true, 0));
    hostComputer.takeEvents();

    send(Action::shortcut(CONTROL, KEY_C, false, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "release key 0xe2",
        "press key 0xe0",
        "press key 0x06",
        "release key 0x06",
        "release key 0xe0",
    }));
}

// ============================================================================
// Consumer and system control
// ============================================================================

TEST_F(ActionSenderTest, ConsumerControlPressesAndReleases) {
    send(Action::consumerControl(0x00e9));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"press consumer control 0x00e9", "release consumer control"}));
}

TEST_F(ActionSenderTest, SystemControlPressesAndReleases) {
    send(Action::systemControl(0x82));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"press system control 0x82", "release system control"}));
}

TEST_F(ActionSenderTest, ConsumerControlReleasesHeldModifiersFirst) {
    send(Action::mouse(CONTROL, 0, 0, 0, 1, 0));
    hostComputer.takeEvents();

    send(Action::consumerControl(0x00e9));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "release key 0xe0",
        "press consumer control 0x00e9",
        "release consumer control",
    }));
}

// ============================================================================
// Mouse
// ============================================================================

TEST_F(ActionSenderTest, MouseClicksButtons) {
    send(Action::mouse(0, 0x01, 0, 0, 0, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"click mouse 0x01"}));
}

TEST_F(ActionSenderTest, MouseMovesAndScrolls) {
    send(Action::mouse(0, 0, 10, -20, 0, 0));
    send(Action::mouse(0, 0, 0, 0, -3, 2));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"move mouse 10 -20 0 0", "move mouse 0 0 -3 2"}));
}

TEST_F(ActionSenderTest, MouseClicksBeforeMoving) {
    send(Action::mouse(0, 0x02, 5, 5, 0, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"click mouse 0x02", "move mouse 5 5 0 0"}));
}

TEST_F(ActionSenderTest, MouseWithModifiersHoldsThemAcrossRepeats) {
    // Like fast scrolling with control held, to zoom
    send(Action::mouse(CONTROL, 0, 0, 0, 1, 0));
    runForMs(50);
    send(Action::mouse(CONTROL, 0, 0, 0, 1, 0));
    runForMs(50);
    send(Action::mouse(CONTROL, 0, 0, 0, 1, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe0",
        "move mouse 0 0 1 0",
        "move mouse 0 0 1 0",
        "move mouse 0 0 1 0",
    }));

    runForMs(250);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe0"}));
}

TEST_F(ActionSenderTest, MouseMovementBeyondReportRangeIsSentOneReportPerUpdate) {
    send(Action::mouse(0, 0, 300, -200, 1, 0));

    // The first report also scrolls
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"move mouse 127 -127 1 0"}));
    EXPECT_TRUE(sender.isBusy());

    sender.update();
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"move mouse 127 -73 0 0"}));
    EXPECT_TRUE(sender.isBusy());

    sender.update();
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"move mouse 46 0 0 0"}));
    EXPECT_FALSE(sender.isBusy());

    sender.update();
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ActionSenderTest, MouseMovementIsAddedToMovementStillBeingSent) {
    send(Action::mouse(0, 0, 200, 0, 0, 0));
    send(Action::mouse(0, 0, -50, 10, 0, 0));
    runForMs(10);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"move mouse 127 0 0 0", "move mouse 23 10 0 0"}));
}

TEST_F(ActionSenderTest, MouseMovementKeepsModifiersHeldUntilHoldTimeAfterLastReport) {
    send(Action::mouse(SHIFT, 0, 0, 400, 0, 0));
    runForMs(3);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe1",
        "move mouse 0 127 0 0",
        "move mouse 0 127 0 0",
        "move mouse 0 127 0 0",
        "move mouse 0 19 0 0",
    }));

    runForMs(249);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    runForMs(1);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe1"}));
}

TEST_F(ActionSenderTest, MouseMovementWithZeroModifierHoldTimeHoldsModifiersAcrossReports) {
    sender.setModifierHoldMs(0);

    send(Action::mouse(SHIFT, 0, 200, 0, 0, 0));
    runForMs(5);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe1",
        "move mouse 127 0 0 0",
        "move mouse 73 0 0 0",
        "release key 0xe1",
    }));
}

TEST_F(ActionSenderTest, MouseMovementPressesModifiersAgainWhenReleasedByOtherAction) {
    send(Action::mouse(CONTROL, 0, 300, 0, 0, 0));
    send(Action::consumerControl(0x00e9));
    sender.update();

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "press key 0xe0",
        "move mouse 127 0 0 0",
        "release key 0xe0",
        "press consumer control 0x00e9",
        "release consumer control",
        "press key 0xe0",
        "move mouse 127 0 0 0",
    }));
}

TEST_F(ActionSenderTest, ZeroModifierHoldTimeReleasesRightAfterAction) {
    sender.setModifierHoldMs(0);

    send(Action::mouse(SHIFT, 0x01, 0, 0, 0, 0));
    sender.update();

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"press key 0xe1", "click mouse 0x01", "release key 0xe1"}));
}

// ============================================================================
// Typing
// ============================================================================

TEST_F(ActionSenderTest, TypesOneCharacterPerUpdateWithTypingDelay) {
    send(Action::type("abc"));
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    // The first character isn't delayed
    sender.update();
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"type 'a'"}));

    runForMs(9);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    runForMs(1);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"type 'b'"}));

    runForMs(10);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"type 'c'"}));

    runForMs(100);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ActionSenderTest, TypeReleasesHeldModifiers) {
    send(Action::shortcut(ALT, KEY_TAB, true, 0));
    hostComputer.takeEvents();

    send(Action::type("a"));
    sender.update();

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe2", "type 'a'"}));
}

TEST_F(ActionSenderTest, TypingWaitsWhileModifiersAreHeld) {
    send(Action::type("ab"));
    sender.update();
    hostComputer.takeEvents();

    // Control would turn the remaining characters into shortcuts
    send(Action::mouse(CONTROL, 0, 0, 0, 1, 0));
    runForMs(249);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"press key 0xe0", "move mouse 0 0 1 0"}));

    runForMs(1);

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"release key 0xe0", "type 'b'"}));
}

TEST_F(ActionSenderTest, IsBusyUntilAllCharactersAreTyped) {
    EXPECT_FALSE(sender.isBusy());

    send(Action::type("ab"));
    EXPECT_TRUE(sender.isBusy());

    sender.update();
    EXPECT_TRUE(sender.isBusy());

    runForMs(10);
    EXPECT_FALSE(sender.isBusy());
}

TEST_F(ActionSenderTest, IsNotBusyWithModifiersHeld) {
    send(Action::shortcut(ALT, KEY_TAB, true, 0));

    EXPECT_FALSE(sender.isBusy());
}

// ============================================================================
// MIDI notes
// ============================================================================

TEST_F(ActionSenderTest, MidiNoteIsEndedAfterItsDuration) {
    send(Action::midiNote(10, 60, 100, 50));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note on 10 60 100"}));

    runForMs(49);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    runForMs(1);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note off 10 60"}));

    runForMs(100);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ActionSenderTest, MidiNoteWithoutDurationIsEndedRightAway) {
    send(Action::midiNote(1, 36, 127, 0));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note on 1 36 127", "midi note off 1 36"}));

    runForMs(10);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ActionSenderTest, ReplayingMidiNoteEndsTheEarlierOneFirst) {
    send(Action::midiNote(1, 60, 100, 50));
    runForMs(20);
    send(Action::midiNote(1, 60, 80, 50));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "midi note on 1 60 100",
        "midi note off 1 60",
        "midi note on 1 60 80",
    }));

    runForMs(49);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    runForMs(1);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note off 1 60"}));
}

TEST_F(ActionSenderTest, MidiNotesOnOtherChannelsOrNotesPlayTogether) {
    send(Action::midiNote(1, 60, 100, 30));
    send(Action::midiNote(1, 64, 100, 20));
    send(Action::midiNote(2, 60, 100, 10));
    hostComputer.takeEvents();

    runForMs(10);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note off 2 60"}));

    runForMs(10);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note off 1 64"}));

    runForMs(10);
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note off 1 60"}));
}

// Notes don't reach the keyboard, so a window switcher kept open by held modifiers stays open
TEST_F(ActionSenderTest, MidiNoteKeepsHeldModifiers) {
    send(Action::shortcut(ALT, KEY_TAB, true, 0));
    hostComputer.takeEvents();

    send(Action::midiNote(1, 60, 100, 10));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note on 1 60 100"}));
}

TEST_F(ActionSenderTest, HeldMidiNotePlaysUntilNoteOff) {
    send(Action::heldMidiNote(1, 60, 127));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note on 1 60 127"}));

    runForMs(100'000);
    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);

    send(Action::midiNoteOff(1, 60));
    EXPECT_EQ(hostComputer.takeEvents(), (Events{"midi note off 1 60"}));
}

TEST_F(ActionSenderTest, NoteOffForEndedMidiNoteSendsNothing) {
    send(Action::midiNote(1, 60, 100, 10));
    runForMs(10);
    hostComputer.takeEvents();

    send(Action::midiNoteOff(1, 60));
    send(Action::midiNoteOff(1, 61));

    EXPECT_EQ(hostComputer.takeEvents(), NO_EVENTS);
}

TEST_F(ActionSenderTest, ReplayingHeldMidiNoteEndsTheEarlierOneFirst) {
    send(Action::heldMidiNote(1, 60, 127));
    send(Action::heldMidiNote(1, 60, 127));
    send(Action::midiNoteOff(1, 60));

    EXPECT_EQ(hostComputer.takeEvents(), (Events{
        "midi note on 1 60 127",
        "midi note off 1 60",
        "midi note on 1 60 127",
        "midi note off 1 60",
    }));
}

TEST_F(ActionSenderTest, IsNotBusyWhileMidiNotesPlay) {
    send(Action::midiNote(1, 60, 100, 50));

    EXPECT_FALSE(sender.isBusy());
}
