#pragma once

#include <functional>
#include <vector>

#include <tfw/hal/time/Clock.h>

#include "mkp/components/macros/Macro.h"

namespace mkp::components::keybindings {
    // Runs the steps of sequence macros over time, as update() is called. Before each step, it waits until the
    // earlier steps are done typing, and then for the delay of the step. Several sequences can run at the same time,
    // but a sequence that is already running isn't started again.
    class SequenceRunner {
    public:
        // Runs the macro of a step
        using StepExecutor = std::function<void(uint16_t macroId)>;
        // Whether earlier actions are still being sent, which steps wait for
        using BusyCheck = std::function<bool()>;

        SequenceRunner(const tfw::hal::time::Clock& clock, BusyCheck isBusy, StepExecutor executeStep);

        void start(uint16_t sequenceId, const std::vector<macros::SequenceStep>& steps);

        // Runs the steps that are due. Called continuously.
        void update();

        void stopAll();

        bool isRunning(uint16_t sequenceId) const;

    private:
        struct Run {
            uint16_t sequenceId;
            std::vector<macros::SequenceStep> steps;
            size_t nextStep = 0;
            // The delay of the next step starts once the earlier steps are done
            bool waitingForIdle = true;
            uint64_t nextStepDueUs = 0;
        };

        // Runs the steps of the run that are due, and returns whether it has run all of them
        bool advance(Run& run);

        const tfw::hal::time::Clock& clock;
        BusyCheck isBusy;
        StepExecutor executeStep;
        std::vector<Run> runs;
    };
}
