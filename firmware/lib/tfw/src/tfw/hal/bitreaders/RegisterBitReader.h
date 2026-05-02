#pragma once

#include "BitReader.h"

#include <tfw/utils/registers.h>

namespace tfw::utils::bitreaders {
    class RegisterBitReader final : public BitReader {
    public:
        RegisterBitReader(const tfw::utils::registers::Register& reg, std::uint8_t bitNumber, Mode mode);
        ~RegisterBitReader() override;

        bool read() override;

    private:

        const tfw::utils::registers::Register& reg;
        const std::uint8_t bitNumber;
        const Mode mode;
    };
}