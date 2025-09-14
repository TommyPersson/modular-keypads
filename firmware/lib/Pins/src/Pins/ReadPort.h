#pragma once

#include <cstdint>

class ReadPort {
public:
    virtual ~ReadPort() {}

    virtual std::uint8_t read() const;
    virtual std::uint8_t read(std::uint8_t pinNumber) const;
};