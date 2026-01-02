#include <Arduino.h>

#include <cstdint>

#include "OutputPin.h"

namespace utils::pins {
    OutputPin::OutputPin(const std::uint8_t pinNumber) :
        pinNumber(pinNumber) {
    }

    void OutputPin::init() const {
        pinMode(pinNumber, OUTPUT);
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
}