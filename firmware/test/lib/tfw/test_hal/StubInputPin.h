#pragma once

#include <cstdint>
#include <tfw/hal/gpio/InputPin.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::gpio::test {
    struct InputPinValueChangedEvent {
        uint8_t pinNumber;
        uint8_t value;
    };

    /**
     * Stub InputPin implementation for testing GPIO interactions.
     * Provides an observable for value changes.
     */
    class StubInputPin : public InputPin {
    public:
        explicit StubInputPin(uint8_t pinNumber)
            : InputPin(pinNumber),
              _value(0) {
        }

        void setup() const override {
            // No-op for testing
        }

        void setupInterrupt(uint8_t mode) override {
            // No-op for testing
            (void)mode;
        }

        void checkForInterrupt() override {
            // No-op for testing
        }

        uint8_t read() const override {
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
            _valueChangedSubject.notify({.pinNumber = pinNumber, .value = value});
        }

    private:
        uint8_t _value;
        utils::observables::Observable<InputPinInterruptEvent> _interruptObservable;
        mutable utils::observables::Subject<InputPinValueChangedEvent> _valueChangedSubject;
    };
}
