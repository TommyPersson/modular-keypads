#include "RegisterBitReader.h"

#include <Registers/Register.h>

std::shared_ptr<BitReader> BitReader::forRegister(const Register& reg, const std::uint8_t bitNumber, const BitReaderMode mode) {
    return std::make_shared<RegisterBitReader>(reg, bitNumber, mode);
}

RegisterBitReader::RegisterBitReader(const Register& reg, const std::uint8_t bitNumber, const BitReaderMode mode) :
    reg(reg), bitNumber(bitNumber), mode(mode) {
    assert(bitNumber < 8);
}

RegisterBitReader::~RegisterBitReader() { // NOLINT(*-use-equals-default)
}

bool RegisterBitReader::read() {
    const auto value = reg.read();

    auto bitValue = (value >> bitNumber) & 0x01;

    if (mode == BitReaderMode::Inverted) {
        if (bitValue == 1) {
            bitValue = 0;
        } else {
            bitValue = 1;
        }
    }

    return bitValue;
}
