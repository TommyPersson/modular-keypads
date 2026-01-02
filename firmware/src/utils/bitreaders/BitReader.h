#pragma once

#include "Registers/Registers.h"

namespace utils::bitreaders {
    enum class BitReaderMode {
        Normal,
        Inverted,
    };

    class BitReader {
    public:
        virtual ~BitReader() = default;
        virtual bool read() = 0;

        static std::shared_ptr<utils::bitreaders::BitReader> forRegister(const Register& reg, std::uint8_t bitNumber, BitReaderMode mode = BitReaderMode::Normal);
    };
}