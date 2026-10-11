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

    runs.push_back(Run{.sequenceId = sequenceId, .steps = steps});

    // Steps without a delay run right away, rather than on the next update
    update();
}

void SequenceRunner::update() {
    std::erase_if(runs, [this](Run& run) { return advance(run); });
}

bool SequenceRunner::advance(Run& run) {
    while (run.nextStep < run.steps.size()) {
        const auto& step = run.steps[run.nextStep];

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

        executeStep(step.macroId);
        run.nextStep++;
        run.waitingForIdle = true;
    }

    return true;
}

void SequenceRunner::stopAll() {
    runs.clear();
}

bool SequenceRunner::isRunning(const uint16_t sequenceId) const {
    return std::ranges::any_of(runs, [sequenceId](const Run& run) { return run.sequenceId == sequenceId; });
}
