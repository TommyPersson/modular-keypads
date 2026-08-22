#pragma once

#include <cstdint>
#include <stdexcept>
#include <tfw/hal/gpio/OutputPin.h>
#include <tfw/hal/time/Clock.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::gpio::test {
    struct OutputPinStateChangedEvent {
        uint8_t pinNumber;
        uint8_t state;
        uint64_t timestampNs = 0;  // Timestamp in nanoseconds when state changed
    };

    /**
     * Stub OutputPin implementation for testing GPIO interactions.
     * Notifies observers when state changes occur with timestamps.
     * Enforces initialization: set() calls throw until init() is called.
     */
    class StubOutputPin : public OutputPin {
    public:
        explicit StubOutputPin(uint8_t pinNumber, tfw::hal::time::Clock& clock, uint8_t modeFlags = 0)
            : OutputPin(pinNumber, modeFlags), _clock(clock) {
        }

        void init() const override {
            _initialized = true;
        }

        void set(uint8_t state) const override {
            if (!_initialized) {
                throw std::logic_error("StubOutputPin::set() called before init()");
            }
            _stateChangedSubject.notify({
                .pinNumber = pinNumber,
                .state = state,
                .timestampNs = _clock.nanos()
            });
        }

        void setHigh() const override {
            set(1);
        }

        void setLow() const override {
            set(0);
        }

        utils::observables::Observable<OutputPinStateChangedEvent>& onStateChanged() const {
            return _stateChangedSubject;
        }

    private:
        tfw::hal::time::Clock& _clock;
        mutable bool _initialized = false;
        mutable utils::observables::Subject<OutputPinStateChangedEvent> _stateChangedSubject;
    };
}
