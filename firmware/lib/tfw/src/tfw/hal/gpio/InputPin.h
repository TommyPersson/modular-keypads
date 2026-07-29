#pragma once

#include <cstdint>
#include <memory>

#include "tfw/utils/observables/Observable.h"

namespace tfw::hal::gpio {
    class InputPin;

    struct InputPinInterruptEvent {
        InputPin* pin;
    };

    class InputPin {
    public:
        explicit InputPin(std::uint8_t pinNumber);
        virtual ~InputPin() = default;

        virtual void setup() const = 0;
        virtual void setupInterrupt(uint8_t mode) = 0;
        virtual void checkForInterrupt() = 0;

        virtual std::uint8_t read() const = 0;
        virtual std::uint16_t readAnalog() const = 0;

        virtual utils::observables::Observable<InputPinInterruptEvent>& onInterruptEvent() = 0;

        static std::unique_ptr<InputPin> physical(std::uint8_t pinNumber);
        static std::unique_ptr<InputPin> physical(std::uint8_t pinNumber, std::uint8_t modeFlags);

        const std::uint8_t pinNumber;
    };
}
