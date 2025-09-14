#include <Arduino.h>

#include <cstdint>

#include "InputPin.h"

#include "PhysicalInputPin.h"
#include "PortInputPin.h"

InputPin::InputPin(const std::uint8_t pinNumber) : pinNumber(pinNumber) {
}

std::unique_ptr<InputPin> InputPin::physical(std::uint8_t pinNumber) {
    return std::make_unique<PhysicalInputPin>(pinNumber);
}

std::unique_ptr<InputPin> InputPin::physical(std::uint8_t pinNumber, std::uint8_t modeFlags) {
    return std::make_unique<PhysicalInputPin>(pinNumber, modeFlags);
}

std::unique_ptr<InputPin> InputPin::port(std::uint8_t pinNumber, const ReadPort& port) {
    return std::make_unique<PortInputPin>(pinNumber, port);
}
