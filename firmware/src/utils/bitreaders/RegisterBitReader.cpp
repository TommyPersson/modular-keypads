#include "RegisterBitReader.h"

#include <Registers/Register.h>

namespace utils::bitreaders {
    RegisterBitReader::RegisterBitReader(
        const utils::registers::Register& reg,
        const std::uint8_t bitNumber,
        const Mode mode
    ) : reg(reg),
        bitNumber(bitNumber),
        mode(mode) {
        assert(bitNumber < 8);
    }

    RegisterBitReader::~RegisterBitReader() = default;

    bool RegisterBitReader::read() {
        const auto value = reg.read();

        auto bitValue = (value >> bitNumber) & 0x01;

        if (mode == Mode::Inverted) {
            if (bitValue == 1) {
                bitValue = 0;
            } else {
                bitValue = 1;
            }
        }

        return bitValue;
    }

    std::shared_ptr<BitReader> for_register(
        const utils::registers::Register& reg,
        const std::uint8_t bitNumber,
        const Mode mode
    ) {
        return std::make_shared<RegisterBitReader>(reg, bitNumber, mode);
    }
}