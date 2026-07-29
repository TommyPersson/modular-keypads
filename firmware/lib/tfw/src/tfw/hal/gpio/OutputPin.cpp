#include <Arduino.h>

#include <cstdint>

#include "OutputPin.h"

namespace tfw::hal::gpio {
    OutputPin::OutputPin(const std::uint8_t pinNumber, const std::uint8_t modeFlags) :
        pinNumber(pinNumber), modeFlags(modeFlags) {
    }

    void OutputPin::init() const {
        pinMode(pinNumber, OUTPUT | modeFlags);
    }


    void OutputPin::set(const std::uint8_t state) const {
        digitalWrite(pinNumber, state);
    }

    void OutputPin::setHigh() const {
        digitalWrite(pinNumber, HIGH);
    }

    void OutputPin::setLow() const {
        digitalWrite(pinNumber, LOW);
    }

    std::unique_ptr<OutputPin> OutputPin::physical(std::uint8_t pinNumber, std::uint8_t modeFlags) {
        return std::make_unique<OutputPin>(pinNumber, modeFlags);
    }
}
