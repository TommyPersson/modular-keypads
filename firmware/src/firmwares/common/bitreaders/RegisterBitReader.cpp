#include "RegisterBitReader.h"

#include <Registers/Register.h>

std::shared_ptr<BitReader> BitReader::forRegister(const Register& reg, std::uint8_t bitNumber) {
    return std::make_shared<RegisterBitReader>(reg, bitNumber);
}

RegisterBitReader::RegisterBitReader(const Register& reg, std::uint8_t bitNumber) : reg(reg), bitNumber(bitNumber) {
    assert(bitNumber < 8);
}

RegisterBitReader::~RegisterBitReader() { // NOLINT(*-use-equals-default)
}

bool RegisterBitReader::read() {
    const auto value = reg.read();
    return (value >> bitNumber) & 0x01;
}
