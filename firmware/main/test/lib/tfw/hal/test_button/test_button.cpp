#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "tfw/hal/buttons.h"
#include "../VirtualClock.h"

using namespace tfw::hal::buttons;
using namespace tfw::hal::time::test;

namespace {
    class StubBitReader : public tfw::hal::bitreaders::BitReader {
    public:
        bool read() override { return value; }

        bool value = false;
    };

    class RecordingObserver : public tfw::utils::observables::Observer<ButtonStateChangedEvent> {
    public:
        void observe(const ButtonStateChangedEvent& event) override { events.push_back(event); }

        std::vector<ButtonStateChangedEvent> events;
    };
}

class ButtonTest : public ::testing::Test {
protected:
    void SetUp() override {
        button.onStateChanged().addObserver(&observer);

        // Settles the initial state, which is reported too
        button.update();
        observer.events.clear();
    }

    void TearDown() override {
        button.onStateChanged().removeObserver(&observer);
    }

    void press() {
        bitReader->value = true;
        button.update();
    }

    void release() {
        bitReader->value = false;
        button.update();
    }

    void holdMs(const uint32_t ms) {
        clock.advanceMs(ms);
        button.update();
    }

    std::vector<ButtonState> toStates() const {
        std::vector<ButtonState> states;
        for (const auto& event : observer.events) {
            states.push_back(event.toState);
        }
        return states;
    }

    VirtualClock clock;
    std::shared_ptr<StubBitReader> bitReader = std::make_shared<StubBitReader>();
    Button button{1, bitReader, clock};
    RecordingObserver observer;
};

TEST_F(ButtonTest, ReportsInitialState) {
    Button other{2, bitReader, clock};
    RecordingObserver otherObserver;
    other.onStateChanged().addObserver(&otherObserver);

    other.update();

    ASSERT_EQ(otherObserver.events.size(), 1);
    EXPECT_EQ(otherObserver.events[0].fromState, ButtonState::UNKNOWN);
    EXPECT_EQ(otherObserver.events[0].toState, ButtonState::UNPRESSED);

    other.onStateChanged().removeObserver(&otherObserver);
}

TEST_F(ButtonTest, ShortPressReportsPressedAndUnpressed) {
    press();
    holdMs(499);
    release();

    EXPECT_EQ(toStates(), (std::vector{ButtonState::PRESSED, ButtonState::UNPRESSED}));
}

TEST_F(ButtonTest, HoldingReportsLongPressed) {
    press();
    holdMs(500);

    EXPECT_EQ(toStates(), (std::vector{ButtonState::PRESSED, ButtonState::LONG_PRESSED}));
    EXPECT_EQ(button.getCurrentState(), ButtonState::LONG_PRESSED);
    EXPECT_EQ(observer.events.back().fromState, ButtonState::PRESSED);
    EXPECT_EQ(observer.events.back().buttonNumber, 1);
}

TEST_F(ButtonTest, LongPressIsReportedOnce) {
    press();
    holdMs(500);
    holdMs(500);
    holdMs(2000);

    EXPECT_EQ(toStates(), (std::vector{ButtonState::PRESSED, ButtonState::LONG_PRESSED}));
}

TEST_F(ButtonTest, ReleaseAfterLongPressReportsUnpressed) {
    press();
    holdMs(600);
    release();

    EXPECT_EQ(toStates(), (std::vector{ButtonState::PRESSED, ButtonState::LONG_PRESSED, ButtonState::UNPRESSED}));
    EXPECT_EQ(observer.events.back().fromState, ButtonState::LONG_PRESSED);
}

TEST_F(ButtonTest, HoldTimeIsMeasuredPerPress) {
    press();
    holdMs(400);
    release();

    press();
    holdMs(400);

    EXPECT_EQ(toStates(), (std::vector{ButtonState::PRESSED, ButtonState::UNPRESSED, ButtonState::PRESSED}));

    holdMs(100);

    EXPECT_EQ(button.getCurrentState(), ButtonState::LONG_PRESSED);
}

TEST_F(ButtonTest, TimeWhileReleasedDoesNotCount) {
    clock.advanceMs(1000);
    release();

    press();

    EXPECT_EQ(toStates(), (std::vector{ButtonState::PRESSED}));
}

TEST_F(ButtonTest, LongPressDurationIsConfigurable) {
    Button quick{2, bitReader, clock, 100'000};
    RecordingObserver quickObserver;
    quick.onStateChanged().addObserver(&quickObserver);
    quick.update();

    bitReader->value = true;
    quick.update();
    clock.advanceMs(100);
    quick.update();

    EXPECT_EQ(quick.getCurrentState(), ButtonState::LONG_PRESSED);

    quick.onStateChanged().removeObserver(&quickObserver);
}
