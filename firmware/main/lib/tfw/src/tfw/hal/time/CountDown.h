#pragma once

#include <cstdint>
#include <functional>

namespace tfw::hal::time {
    class Clock;

    using CountDownCallback = std::function<void()>;

    /**
     * One-shot countdown driven by explicit calls to update().
     *
     * A countdown is created stopped. Once start() is called, the callback is
     * invoked once by the first update() at or after the duration has passed,
     * after which the countdown stops. It can be started again to repeat.
     *
     * Create instances through Clock::createCountDown().
     */
    class CountDown {
    public:
        CountDown(const Clock& clock, uint64_t durationUs, CountDownCallback callback);
        CountDown(const CountDown&) = delete;
        CountDown& operator=(const CountDown&) = delete;
        ~CountDown() = default;

        void update();

        /** Start counting down from now. Restarts the countdown if it is already running. */
        void start();

        /** Stop the countdown without invoking the callback. */
        void stop();

        bool isRunning() const;

        uint64_t getDurationUs() const;
        /** Time left until the callback is invoked, or 0 when not running. */
        uint64_t getRemainingUs() const;

    private:
        const Clock& clock;
        const uint64_t durationUs;
        const CountDownCallback callback;

        bool running = false;
        uint64_t startedAtUs = 0;
    };
}
