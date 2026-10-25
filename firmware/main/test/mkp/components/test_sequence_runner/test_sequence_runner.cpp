#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

#include "mkp/components/keybindings/SequenceRunner.h"
#include "mkp/components/keybindings/SequenceRunner.cpp"
#include "../../../lib/tfw/hal/VirtualClock.h"

using namespace mkp::components::keybindings;
using namespace mkp::components::macros;
using namespace tfw::hal::time::test;

using MacroIds = std::vector<uint16_t>;

class SequenceRunnerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Like a device that has been running for a while
        clock.setTimeUs(10'000'000);
    }

    // Advances time in steps, updating the runner after each step, like the main loop does
    void runForMs(const uint32_t durationMs) {
        for (uint32_t elapsedMs = 0; elapsedMs < durationMs; elapsedMs++) {
            clock.advanceMs(1);
            runner.update();
        }
    }

    MacroIds takeExecuted() {
        auto result = executed;
        executed.clear();
        return result;
    }

    VirtualClock clock;
    bool busy = false;
    // Macros that start typing when they're run, which keeps the connection busy until the test says it's done
    MacroIds typingMacros;
    MacroIds executed;
    SequenceRunner runner{
        clock,
        [this] { return busy; },
        [this](const uint16_t macroId) {
            executed.push_back(macroId);
            if (std::ranges::find(typingMacros, macroId) != typingMacros.end()) {
                busy = true;
            }
        }
    };
};

TEST_F(SequenceRunnerTest, StepsWithoutDelayRunRightAway) {
    runner.start(1, {{10, 0}, {11, 0}, {12, 0}});

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 11, 12}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, StepsWaitForTheirDelay) {
    runner.start(1, {{10, 0}, {11, 100}, {12, 50}});
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));

    runForMs(99);
    EXPECT_EQ(takeExecuted(), MacroIds{});

    runForMs(1);
    EXPECT_EQ(takeExecuted(), (MacroIds{11}));

    runForMs(49);
    EXPECT_EQ(takeExecuted(), MacroIds{});

    runForMs(1);
    EXPECT_EQ(takeExecuted(), (MacroIds{12}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, FirstStepCanBeDelayed) {
    runner.start(1, {{10, 20}});
    EXPECT_EQ(takeExecuted(), MacroIds{});

    runForMs(20);
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));
}

TEST_F(SequenceRunnerTest, DelayStartsWhenEarlierStepsAreDoneTyping) {
    typingMacros = {10};
    runner.start(1, {{10, 0}, {11, 100}});
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));

    // Like the first step typing a text for a while
    runForMs(500);
    EXPECT_EQ(takeExecuted(), MacroIds{});

    // The delay starts with the update that finds the typing done
    busy = false;
    runner.update();
    runForMs(99);
    EXPECT_EQ(takeExecuted(), MacroIds{});

    runForMs(1);
    EXPECT_EQ(takeExecuted(), (MacroIds{11}));
}

TEST_F(SequenceRunnerTest, StepWithoutDelayWaitsUntilEarlierStepsAreDoneTyping) {
    busy = true;
    runner.start(1, {{10, 0}});
    runForMs(10);
    EXPECT_EQ(takeExecuted(), MacroIds{});

    busy = false;
    runForMs(1);
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));
}

TEST_F(SequenceRunnerTest, RunningSequenceIsNotStartedAgain) {
    runner.start(1, {{10, 0}, {11, 100}});
    runForMs(50);
    runner.start(1, {{10, 0}, {11, 100}});
    runForMs(50);

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 11}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, FinishedSequenceCanBeStartedAgain) {
    runner.start(1, {{10, 0}});
    runner.start(1, {{10, 0}});

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 10}));
}

TEST_F(SequenceRunnerTest, DifferentSequencesRunAtTheSameTime) {
    runner.start(1, {{10, 0}, {11, 100}});
    runForMs(50);
    runner.start(2, {{20, 0}, {21, 100}});

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 20}));

    runForMs(50);
    EXPECT_EQ(takeExecuted(), (MacroIds{11}));

    runForMs(50);
    EXPECT_EQ(takeExecuted(), (MacroIds{21}));
}

TEST_F(SequenceRunnerTest, StopAllStopsRunningSequences) {
    runner.start(1, {{10, 0}, {11, 100}});
    takeExecuted();

    runner.stopAll();
    runForMs(200);

    EXPECT_EQ(takeExecuted(), MacroIds{});
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, StopStopsOnlyThatSequence) {
    runner.start(1, {{10, 0}, {11, 100}});
    runner.start(2, {{20, 0}, {21, 100}});
    takeExecuted();

    runner.stop(1);
    runForMs(200);

    EXPECT_EQ(takeExecuted(), (MacroIds{21}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, StoppedSequenceCanBeStartedAgain) {
    runner.start(1, {{10, 0}, {11, 100}});
    runner.stop(1);
    runner.start(1, {{10, 0}, {11, 100}});
    runForMs(100);

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 10, 11}));
}

TEST_F(SequenceRunnerTest, SequenceWithoutStepsDoesNothing) {
    runner.start(1, {});

    EXPECT_EQ(takeExecuted(), MacroIds{});
    EXPECT_FALSE(runner.isRunning(1));
}

// Goes back to the step at the index, a number of times
static SequenceStep loop(const uint8_t target, const uint16_t repeats, const uint32_t delayMs = 0) {
    return SequenceStep{.macroId = 0, .delayMs = delayMs, .loopTarget = target, .loopRepeats = repeats};
}

TEST_F(SequenceRunnerTest, LoopGoesBackToEarlierStep) {
    runner.start(1, {{10, 0}, {11, 0}, {12, 0}, loop(1, 2), {13, 0}});
    runForMs(10);

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 11, 12, 11, 12, 11, 12, 13}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, LoopCanBeLastStep) {
    runner.start(1, {{10, 0}, loop(0, 1)});
    runForMs(10);

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 10}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, LoopWaitsForItsDelayBeforeGoingBack) {
    runner.start(1, {{10, 0}, loop(0, 1, 100), {11, 0}});
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));

    runForMs(99);
    EXPECT_EQ(takeExecuted(), MacroIds{});

    // The next step runs right away once the loop is done, without waiting for the delay again
    runForMs(2);
    EXPECT_EQ(takeExecuted(), (MacroIds{10, 11}));
}

TEST_F(SequenceRunnerTest, LoopWaitsUntilEarlierStepsAreDoneTyping) {
    typingMacros = {10};
    runner.start(1, {{10, 0}, loop(0, 1)});
    runForMs(10);
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));

    busy = false;
    runForMs(2);
    EXPECT_EQ(takeExecuted(), (MacroIds{10}));
}

TEST_F(SequenceRunnerTest, InnerLoopRepeatsOnEachPassOfOuterLoop) {
    runner.start(1, {{10, 0}, {11, 0}, loop(1, 1), loop(0, 1)});
    runForMs(10);

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 11, 11, 10, 11, 11}));
    EXPECT_FALSE(runner.isRunning(1));
}

TEST_F(SequenceRunnerTest, LoopThatDoesNotGoBackIsSkipped) {
    runner.start(1, {{10, 0}, loop(1, 5), {11, 0}});
    runForMs(10);

    EXPECT_EQ(takeExecuted(), (MacroIds{10, 11}));
}

TEST_F(SequenceRunnerTest, StopAllStopsLoopingSequences) {
    runner.start(1, {{10, 100}, loop(0, 100)});
    runForMs(250);
    EXPECT_EQ(takeExecuted(), (MacroIds{10, 10}));

    runner.stopAll();
    runForMs(500);

    EXPECT_EQ(takeExecuted(), MacroIds{});
}
