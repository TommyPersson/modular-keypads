#pragma once

#include <cstdint>
#include <tfw/hal/gpio/OutputPin.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::gpio::test {
    struct OutputPinStateChangedEvent {
        uint8_t pinNumber;
        uint8_t state;
    };

    /**
     * Stub OutputPin implementation for testing GPIO interactions.
     * Notifies observers when state changes occur.
     */
    class StubOutputPin : public OutputPin {
    public:
        explicit StubOutputPin(uint8_t pinNumber, uint8_t modeFlags = 0)
            : OutputPin(pinNumber, modeFlags) {
        }

        void init() const override {
            // No-op in test environment
        }

        void set(uint8_t state) const override {
            _stateChangedSubject.notify({.pinNumber = pinNumber, .state = state});
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
        mutable utils::observables::Subject<OutputPinStateChangedEvent> _stateChangedSubject;
    };
}
