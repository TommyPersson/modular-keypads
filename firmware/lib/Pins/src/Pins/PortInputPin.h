#pragma once

#include "InputPin.h"
#include "ReadPort.h"

class PortInputPin final : public InputPin {
public:
    explicit PortInputPin(std::uint8_t pinNumber, const ReadPort& port);

    void init() const override;
    std::uint8_t read() const override;

private:
    const ReadPort& port;
};