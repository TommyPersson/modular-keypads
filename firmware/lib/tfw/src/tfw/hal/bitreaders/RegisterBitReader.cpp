#include "RegisterBitReader.h"

namespace tfw::hal::bitreaders {
    RegisterBitReader::RegisterBitReader(
        const tfw::utils::registers::Register& reg,
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
        const tfw::utils::registers::Register& reg,
        const std::uint8_t bitNumber,
        const Mode mode
    ) {
        return std::make_shared<RegisterBitReader>(reg, bitNumber, mode);
    }
}