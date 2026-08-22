#pragma once

#include <cstdint>
#include <stdexcept>
#include <tfw/hal/gpio/InputPin.h>
#include <tfw/hal/time.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::gpio::test {
    struct InputPinValueChangedEvent {
        uint8_t pinNumber;
        uint8_t value;
        uint64_t timestampNs = 0;  // Timestamp in nanoseconds when value changed
    };

    /**
     * Stub InputPin implementation for testing GPIO interactions.
     * Provides an observable for value changes with timestamps.
     * Enforces initialization: read() calls throw until setup() is called.
     */
    class StubInputPin : public InputPin {
    public:
        explicit StubInputPin(uint8_t pinNumber, tfw::hal::time::Clock& clock)
            : InputPin(pinNumber),
              _clock(clock),
              _value(0),
              _initialized(false) {
        }

        void setup() const override {
            _initialized = true;
        }

        void setupInterrupt(uint8_t mode) override {
            // No-op for testing
            (void)mode;
        }

        void checkForInterrupt() override {
            // No-op for testing
        }

        uint8_t read() const override {
            if (!_initialized) {
                throw std::logic_error("StubInputPin::read() called before setup()");
            }
            return _value;
        }

        uint16_t readAnalog() const override {
            return 0;
        }

        utils::observables::Observable<InputPinInterruptEvent>& onInterruptEvent() override {
            return _interruptObservable;
        }

        utils::observables::Observable<InputPinValueChangedEvent>& onValueChanged() const {
            return _valueChangedSubject;
        }

        void setValue(uint8_t value) {
            _value = value;
            _valueChangedSubject.notify({
                .pinNumber = pinNumber,
                .value = value,
                .timestampNs = _clock.nanos()
            });
        }

    private:
        tfw::hal::time::Clock& _clock;
        uint8_t _value;
        mutable bool _initialized = false;
        utils::observables::Observable<InputPinInterruptEvent> _interruptObservable;
        mutable utils::observables::Subject<InputPinValueChangedEvent> _valueChangedSubject;
    };
}
