#include "Timer.h"

#include "Clock.h"

using namespace tfw::hal::time;

std::unique_ptr<Timer> Clock::createTimer(const uint64_t intervalUs, TimerCallback callback) const {
    return std::make_unique<Timer>(*this, intervalUs, std::move(callback));
}

Timer::Timer(const Clock& clock, const uint64_t intervalUs, TimerCallback callback)
    : clock(clock),
      intervalUs(intervalUs),
      callback(std::move(callback)),
      lastUpdateUs(clock.micros()) {
}

void Timer::update() {
    const auto now = clock.micros();
    elapsedUs += now - lastUpdateUs;
    lastUpdateUs = now;

    if (intervalUs == 0 || elapsedUs < intervalUs) {
        return;
    }

    const auto intervalsElapsed = elapsedUs / intervalUs;
    elapsedUs %= intervalUs;

    if (callback) {
        callback(
            {
                .intervalUs = intervalUs,
                .intervalsElapsed = intervalsElapsed,
            }
        );
    }
}

void Timer::reset() {
    lastUpdateUs = clock.micros();
    elapsedUs = 0;
}

uint64_t Timer::getIntervalUs() const {
    return intervalUs;
}

uint64_t Timer::getElapsedUs() const {
    return elapsedUs;
}
