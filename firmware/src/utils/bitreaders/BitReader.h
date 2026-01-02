#pragma once

#include "Registers/Registers.h"

namespace utils::bitreaders {
    enum class Mode {
        Normal,
        Inverted,
    };

    class BitReader {
    public:
        virtual ~BitReader() = default;
        virtual bool read() = 0;
    };

    std::shared_ptr<BitReader> for_register(
        const utils::registers::Register& reg,
        std::uint8_t bitNumber,
        Mode mode = Mode::Normal
    );
}
