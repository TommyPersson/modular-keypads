#pragma once

#include <memory>

#include "ReadPort.h"

class InputPin {
public:
    InputPin(std::uint8_t pinNumber);
    virtual ~InputPin() {}

    virtual void init() const = 0;

    virtual std::uint8_t read() const = 0;

    static std::unique_ptr<InputPin> physical(std::uint8_t pinNumber);
    static std::unique_ptr<InputPin> physical(std::uint8_t pinNumber, std::uint8_t modeFlags);
    static std::unique_ptr<InputPin> port(std::uint8_t pinNumber, const ReadPort& port);

    const std::uint8_t pinNumber;
};
