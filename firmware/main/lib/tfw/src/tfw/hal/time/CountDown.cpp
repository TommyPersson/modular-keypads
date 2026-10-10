#include "CountDown.h"

#include "Clock.h"

using namespace tfw::hal::time;

std::unique_ptr<CountDown> Clock::createCountDown(const uint64_t durationUs, CountDownCallback callback) const {
    return std::make_unique<CountDown>(*this, durationUs, std::move(callback));
}

CountDown::CountDown(const Clock& clock, const uint64_t durationUs, CountDownCallback callback)
    : clock(clock),
      durationUs(durationUs),
      callback(std::move(callback)) {
}

void CountDown::update() {
    if (!running || clock.micros() - startedAtUs < durationUs) {
        return;
    }

    // Stopped before the callback, so that it can start the countdown again
    running = false;

    if (callback) {
        callback();
    }
}

void CountDown::start() {
    startedAtUs = clock.micros();
    running = true;
}

void CountDown::stop() {
    running = false;
}

bool CountDown::isRunning() const {
    return running;
}

uint64_t CountDown::getDurationUs() const {
    return durationUs;
}

uint64_t CountDown::getRemainingUs() const {
    if (!running) {
        return 0;
    }

    const auto elapsedUs = clock.micros() - startedAtUs;
    return elapsedUs < durationUs ? durationUs - elapsedUs : 0;
}
