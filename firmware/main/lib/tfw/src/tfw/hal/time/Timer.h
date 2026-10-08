#pragma once

#include <cstdint>
#include <functional>

namespace tfw::hal::time {
    class Clock;

    struct TimerElapsedEvent {
        /** The timer's configured interval in microseconds. */
        uint64_t intervalUs;
        /** Number of whole intervals that passed since the previous notification (>= 1). */
        uint64_t intervalsElapsed;
    };

    using TimerCallback = std::function<void(const TimerElapsedEvent&)>;

    /**
     * Interval timer driven by explicit calls to update().
     *
     * Each update() advances the timer by the time passed since the previous
     * update() (or since creation/reset). When the accumulated time reaches the
     * interval, the callback is invoked once. Any remainder is carried
     * over to the next interval so the timer does not drift.
     *
     * Create instances through Clock::createTimer().
     */
    class Timer {
    public:
        Timer(const Clock& clock, uint64_t intervalUs, TimerCallback callback);
        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;
        ~Timer() = default;

        void update();

        /** Restart the current interval from now. */
        void reset();

        uint64_t getIntervalUs() const;
        uint64_t getElapsedUs() const;

    private:
        const Clock& clock;
        const uint64_t intervalUs;
        const TimerCallback callback;

        uint64_t lastUpdateUs;
        uint64_t elapsedUs = 0;
    };
}
