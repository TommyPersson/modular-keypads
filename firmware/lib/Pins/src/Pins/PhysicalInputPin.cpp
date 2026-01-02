#include "PhysicalInputPin.h"

#include <Arduino.h>



PhysicalInputPin::PhysicalInputPin(const std::uint8_t pinNumber) :
    InputPin(pinNumber),
    modeFlags(0) {
}

PhysicalInputPin::PhysicalInputPin(const std::uint8_t pinNumber, const std::uint8_t modeFlags) :
    InputPin(pinNumber),
    modeFlags(modeFlags) {
}

PhysicalInputPin::~PhysicalInputPin() = default;

void PhysicalInputPin::init() const {
    pinMode(pinNumber, INPUT | modeFlags);
}

std::uint8_t PhysicalInputPin::read() const {
    return digitalRead(pinNumber);
}
