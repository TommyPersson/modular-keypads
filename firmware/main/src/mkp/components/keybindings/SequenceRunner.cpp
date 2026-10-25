#include "SequenceRunner.h"

#include <algorithm>

using namespace mkp::components::keybindings;
using namespace mkp::components::macros;

SequenceRunner::SequenceRunner(const tfw::hal::time::Clock& clock, BusyCheck isBusy, StepExecutor executeStep)
    : clock(clock),
      isBusy(std::move(isBusy)),
      executeStep(std::move(executeStep)) {
}

void SequenceRunner::start(const uint16_t sequenceId, const std::vector<SequenceStep>& steps) {
    // Running it twice at once would interleave the steps, such as when the key is pressed twice by accident
    if (steps.empty() || isRunning(sequenceId)) {
        return;
    }

    runs.push_back(Run{.sequenceId = sequenceId, .steps = steps, .timesLooped = std::vector<uint16_t>(steps.size())});

    // Steps without a delay run right away, rather than on the next update
    update();
}

void SequenceRunner::update() {
    std::erase_if(runs, [this](Run& run) { return advance(run); });
}

bool SequenceRunner::advance(Run& run) {
    while (run.nextStep < run.steps.size()) {
        const auto& step = run.steps[run.nextStep];

        // Loops that are done go on right away, as their delay is for going back. So do loops that don't go back to
        // an earlier step, which would never end.
        if (step.isLoop() && (run.timesLooped[run.nextStep] >= step.loopRepeats || step.loopTarget >= run.nextStep)) {
            run.timesLooped[run.nextStep] = 0;
            run.nextStep++;
            continue;
        }

        if (run.waitingForIdle) {
            if (isBusy()) {
                return false;
            }
            run.nextStepDueUs = clock.micros() + static_cast<uint64_t>(step.delayMs) * 1'000;
            run.waitingForIdle = false;
        }

        if (clock.micros() < run.nextStepDueUs) {
            return false;
        }

        run.waitingForIdle = true;

        if (step.isLoop()) {
            run.timesLooped[run.nextStep]++;
            run.nextStep = step.loopTarget;
            // Goes on with the next update, so that loops without delays don't keep the main loop busy
            return false;
        }

        executeStep(step.macroId);
        run.nextStep++;
    }

    return true;
}

void SequenceRunner::stop(const uint16_t sequenceId) {
    std::erase_if(runs, [sequenceId](const Run& run) { return run.sequenceId == sequenceId; });
}

void SequenceRunner::stopAll() {
    runs.clear();
}

bool SequenceRunner::isRunning(const uint16_t sequenceId) const {
    return std::ranges::any_of(runs, [sequenceId](const Run& run) { return run.sequenceId == sequenceId; });
}
