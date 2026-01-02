#pragma once

#include "BitReader.h"

#include <Registers/Registers.h>

class RegisterBitReader final : public BitReader {
public:
    RegisterBitReader(const Register& reg, std::uint8_t bitNumber, BitReaderMode mode);
    ~RegisterBitReader() override;

    bool read() override;

private:

    const Register& reg;
    const std::uint8_t bitNumber;
    const BitReaderMode mode;
};