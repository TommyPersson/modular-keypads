#pragma once

#include "ReadPort.h"

class InMemoryReadPort final : public ReadPort {
public:
    InMemoryReadPort();
    ~InMemoryReadPort() override;

    void update(std::uint8_t state);

    std::uint8_t read() const override;
    std::uint8_t read(std::uint8_t pinNumber) const override;

private:
    std::uint8_t _state;
};