#include "PhysicalInputPin.h"

#include <Arduino.h>

#include "tfw/hal/logging/Logger.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("PhysicalInputPin");;

    using namespace tfw::hal::gpio;

    std::vector<bool> triggeredInterrupts(GPIO_NUM_MAX);

    void interruptHandler(void* pin) {
        auto inputPin = static_cast<PhysicalInputPin*>(pin);
        triggeredInterrupts[inputPin->pinNumber] = true;
    }
}

namespace tfw::hal::gpio {
    PhysicalInputPin::PhysicalInputPin(const std::uint8_t pinNumber) :
        InputPin(pinNumber),
        modeFlags(0) {
    }

    PhysicalInputPin::PhysicalInputPin(const std::uint8_t pinNumber, const std::uint8_t modeFlags) :
        InputPin(pinNumber),
        modeFlags(modeFlags) {
    }

    PhysicalInputPin::~PhysicalInputPin() = default;

    void PhysicalInputPin::setup() const {
        pinMode(pinNumber, INPUT | modeFlags);
    }

    std::uint8_t PhysicalInputPin::read() const {
        return digitalRead(pinNumber);
    }

    std::uint16_t PhysicalInputPin::readAnalog() const {
        return analogRead(pinNumber);
    }

    void PhysicalInputPin::setupInterrupt(uint8_t mode) {
        attachInterruptArg(pinNumber, interruptHandler, this, mode);
    }

    void PhysicalInputPin::checkForInterrupt() {
        if (triggeredInterrupts[pinNumber]) {
            triggeredInterrupts[pinNumber] = false;

            interruptSubject.notify(InputPinInterruptEvent{ .pin = this });
        }
    }
}
