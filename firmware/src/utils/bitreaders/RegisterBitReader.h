#pragma once

#include "BitReader.h"

#include <Registers/Registers.h>

namespace utils::bitreaders {
    class RegisterBitReader final : public BitReader {
    public:
        RegisterBitReader(const utils::registers::Register& reg, std::uint8_t bitNumber, Mode mode);
        ~RegisterBitReader() override;

        bool read() override;

    private:

        const utils::registers::Register& reg;
        const std::uint8_t bitNumber;
        const Mode mode;
    };
}