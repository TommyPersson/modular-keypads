#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "tfw/hal/time.h"
#include "../VirtualClock.h"

using namespace tfw::hal::time;
using namespace tfw::hal::time::test;

class TimerTest : public ::testing::Test {
protected:
    std::unique_ptr<Timer> createRecordingTimer(const uint64_t intervalUs) {
        return clock.createTimer(intervalUs, [this](const TimerElapsedEvent& event) { events.push_back(event); });
    }

    VirtualClock clock;
    std::vector<TimerElapsedEvent> events;
};

TEST_F(TimerTest, FactoryCreatesTimerWithInterval) {
    auto timer = createRecordingTimer(1000);

    EXPECT_EQ(timer->getIntervalUs(), 1000);
    EXPECT_EQ(timer->getElapsedUs(), 0);
}

TEST_F(TimerTest, DoesNotNotifyBeforeIntervalPassed) {
    auto timer = createRecordingTimer(1000);

    clock.advanceUs(999);
    timer->update();

    EXPECT_TRUE(events.empty());
    EXPECT_EQ(timer->getElapsedUs(), 999);
}

TEST_F(TimerTest, NotifiesWhenIntervalReached) {
    auto timer = createRecordingTimer(1000);

    clock.advanceUs(1000);
    timer->update();

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].intervalUs, 1000);
    EXPECT_EQ(events[0].intervalsElapsed, 1);
    EXPECT_EQ(timer->getElapsedUs(), 0);
}

TEST_F(TimerTest, AccumulatesTimeAcrossUpdates) {
    auto timer = createRecordingTimer(1000);

    clock.advanceUs(400);
    timer->update();
    clock.advanceUs(400);
    timer->update();
    EXPECT_TRUE(events.empty());

    clock.advanceUs(400);
    timer->update();

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(timer->getElapsedUs(), 200);
}

TEST_F(TimerTest, CarriesRemainderToAvoidDrift) {
    auto timer = createRecordingTimer(1000);

    clock.advanceUs(1300);
    timer->update();
    clock.advanceUs(700);
    timer->update();

    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(timer->getElapsedUs(), 0);
}

TEST_F(TimerTest, NotifiesOnceWithCountWhenMultipleIntervalsPassed) {
    auto timer = createRecordingTimer(1000);

    clock.advanceUs(3500);
    timer->update();

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].intervalsElapsed, 3);
    EXPECT_EQ(timer->getElapsedUs(), 500);
}

TEST_F(TimerTest, MeasuresFromCreationTime) {
    clock.advanceUs(5000);
    auto timer = createRecordingTimer(1000);

    timer->update();
    EXPECT_TRUE(events.empty());

    clock.advanceUs(1000);
    timer->update();
    EXPECT_EQ(events.size(), 1);
}

TEST_F(TimerTest, ResetRestartsInterval) {
    auto timer = createRecordingTimer(1000);

    clock.advanceUs(800);
    timer->update();
    timer->reset();

    clock.advanceUs(800);
    timer->update();

    EXPECT_TRUE(events.empty());
    EXPECT_EQ(timer->getElapsedUs(), 800);
}

TEST_F(TimerTest, EmptyCallbackIsSafe) {
    auto timer = clock.createTimer(1000, nullptr);

    clock.advanceUs(1000);
    timer->update();

    EXPECT_EQ(timer->getElapsedUs(), 0);
}

TEST_F(TimerTest, MultipleTimersInvokeTheirOwnCallbacks) {
    int fastCount = 0;
    int slowCount = 0;
    auto fast = clock.createTimer(100, [&fastCount](const TimerElapsedEvent&) { fastCount++; });
    auto slow = clock.createTimer(300, [&slowCount](const TimerElapsedEvent&) { slowCount++; });

    for (int i = 0; i < 6; i++) {
        clock.advanceUs(100);
        fast->update();
        slow->update();
    }

    EXPECT_EQ(fastCount, 6);
    EXPECT_EQ(slowCount, 2);
}

TEST_F(TimerTest, ZeroIntervalNeverNotifies) {
    auto timer = createRecordingTimer(0);

    clock.advanceUs(1000);
    timer->update();

    EXPECT_TRUE(events.empty());
}
