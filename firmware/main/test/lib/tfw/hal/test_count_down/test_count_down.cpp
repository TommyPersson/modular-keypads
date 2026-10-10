#include <gtest/gtest.h>
#include <memory>
#include "tfw/hal/time.h"
#include "../VirtualClock.h"

using namespace tfw::hal::time;
using namespace tfw::hal::time::test;

class CountDownTest : public ::testing::Test {
protected:
    std::unique_ptr<CountDown> createRecordingCountDown(const uint64_t durationUs) {
        return clock.createCountDown(durationUs, [this] { invocations++; });
    }

    VirtualClock clock;
    int invocations = 0;
};

TEST_F(CountDownTest, FactoryCreatesStoppedCountDownWithDuration) {
    auto countDown = createRecordingCountDown(1000);

    EXPECT_EQ(countDown->getDurationUs(), 1000);
    EXPECT_FALSE(countDown->isRunning());
    EXPECT_EQ(countDown->getRemainingUs(), 0);
}

TEST_F(CountDownTest, DoesNotNotifyWhenNotStarted) {
    auto countDown = createRecordingCountDown(1000);

    clock.advanceUs(5000);
    countDown->update();

    EXPECT_EQ(invocations, 0);
}

TEST_F(CountDownTest, DoesNotNotifyBeforeDurationPassed) {
    auto countDown = createRecordingCountDown(1000);
    countDown->start();

    clock.advanceUs(999);
    countDown->update();

    EXPECT_EQ(invocations, 0);
    EXPECT_TRUE(countDown->isRunning());
    EXPECT_EQ(countDown->getRemainingUs(), 1);
}

TEST_F(CountDownTest, NotifiesAndStopsWhenDurationReached) {
    auto countDown = createRecordingCountDown(1000);
    countDown->start();

    clock.advanceUs(1000);
    countDown->update();

    EXPECT_EQ(invocations, 1);
    EXPECT_FALSE(countDown->isRunning());
}

TEST_F(CountDownTest, NotifiesOnlyOnce) {
    auto countDown = createRecordingCountDown(1000);
    countDown->start();

    for (int i = 0; i < 5; i++) {
        clock.advanceUs(1000);
        countDown->update();
    }

    EXPECT_EQ(invocations, 1);
}

TEST_F(CountDownTest, MeasuresFromStartTime) {
    auto countDown = createRecordingCountDown(1000);

    clock.advanceUs(5000);
    countDown->start();
    countDown->update();
    EXPECT_EQ(invocations, 0);

    clock.advanceUs(1000);
    countDown->update();
    EXPECT_EQ(invocations, 1);
}

TEST_F(CountDownTest, StartWhileRunningRestarts) {
    auto countDown = createRecordingCountDown(1000);
    countDown->start();

    clock.advanceUs(800);
    countDown->start();

    clock.advanceUs(800);
    countDown->update();

    EXPECT_EQ(invocations, 0);
    EXPECT_EQ(countDown->getRemainingUs(), 200);
}

TEST_F(CountDownTest, StopCancelsWithoutNotifying) {
    auto countDown = createRecordingCountDown(1000);
    countDown->start();

    clock.advanceUs(500);
    countDown->stop();

    clock.advanceUs(1000);
    countDown->update();

    EXPECT_EQ(invocations, 0);
    EXPECT_FALSE(countDown->isRunning());
}

TEST_F(CountDownTest, CanBeStartedAgainAfterNotifying) {
    auto countDown = createRecordingCountDown(1000);
    countDown->start();
    clock.advanceUs(1000);
    countDown->update();

    countDown->start();
    clock.advanceUs(1000);
    countDown->update();

    EXPECT_EQ(invocations, 2);
}

TEST_F(CountDownTest, CallbackCanRestartCountDown) {
    std::unique_ptr<CountDown> countDown;
    countDown = clock.createCountDown(1000, [&] {
        invocations++;
        countDown->start();
    });
    countDown->start();

    clock.advanceUs(1000);
    countDown->update();

    EXPECT_EQ(invocations, 1);
    EXPECT_TRUE(countDown->isRunning());
}

TEST_F(CountDownTest, ZeroDurationNotifiesOnFirstUpdate) {
    auto countDown = createRecordingCountDown(0);
    countDown->start();

    countDown->update();

    EXPECT_EQ(invocations, 1);
}

TEST_F(CountDownTest, EmptyCallbackIsSafe) {
    auto countDown = clock.createCountDown(1000, nullptr);
    countDown->start();

    clock.advanceUs(1000);
    countDown->update();

    EXPECT_FALSE(countDown->isRunning());
}
