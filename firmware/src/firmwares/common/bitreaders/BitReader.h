#pragma once

#include "Registers/Registers.h"

class BitReader {
public:
    virtual ~BitReader() = default;
    virtual bool read() = 0;

    static std::shared_ptr<BitReader> forRegister(const Register& reg, std::uint8_t bitNumber);
};
