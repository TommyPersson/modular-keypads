#pragma once

#include "InputPin.h"
#include "tfw/utils/observables/Subject.h"

namespace tfw::hal::gpio {
    class PhysicalInputPin final : public InputPin {
    public:
        explicit PhysicalInputPin(std::uint8_t pinNumber);
        explicit PhysicalInputPin(std::uint8_t pinNumber, std::uint8_t modeFlags);
        ~PhysicalInputPin() override;

        void setup() const override;
        void setupInterrupt(uint8_t mode) override;
        void checkForInterrupt() override;

        std::uint8_t read() const override;
        std::uint16_t readAnalog() const override;

        utils::observables::Observable<InputPinInterruptEvent>& onInterruptEvent() override { return interruptSubject; }

    private:
        const std::uint8_t modeFlags;
        utils::observables::Subject<InputPinInterruptEvent> interruptSubject;
    };
}
