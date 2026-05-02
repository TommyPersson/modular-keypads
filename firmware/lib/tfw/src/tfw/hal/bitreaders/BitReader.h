#pragma once

#include "../../utils/registers/Registers.h"

namespace tfw::utils::bitreaders {
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
        const tfw::utils::registers::Register& reg,
        std::uint8_t bitNumber,
        Mode mode = Mode::Normal
    );
}
