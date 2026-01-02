#include <Arduino.h>

#include "InputPin.h"

#include "PhysicalInputPin.h"

InputPin::InputPin(const std::uint8_t pinNumber) : pinNumber(pinNumber) {
}

std::unique_ptr<InputPin> InputPin::physical(std::uint8_t pinNumber) {
    return std::make_unique<PhysicalInputPin>(pinNumber);
}

std::unique_ptr<InputPin> InputPin::physical(std::uint8_t pinNumber, std::uint8_t modeFlags) {
    return std::make_unique<PhysicalInputPin>(pinNumber, modeFlags);
}

