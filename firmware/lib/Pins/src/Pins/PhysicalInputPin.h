#pragma once

#include "InputPin.h"

class PhysicalInputPin final : public InputPin {
public:
    explicit PhysicalInputPin(std::uint8_t pinNumber);
    explicit PhysicalInputPin(std::uint8_t pinNumber, std::uint8_t modeFlags);
    ~PhysicalInputPin() override;

    void init() const override;
    std::uint8_t read() const override;

private:
    const std::uint8_t modeFlags;
};
