#include "InMemoryReadPort.h"

InMemoryReadPort::InMemoryReadPort() :
    _state(0) {
}

InMemoryReadPort::~InMemoryReadPort() {
}

void InMemoryReadPort::update(const std::uint8_t state) {
    this->_state = state;
}

std::uint8_t InMemoryReadPort::read() const {
    return this->_state;
}

std::uint8_t InMemoryReadPort::read(const std::uint8_t pinNumber) const {
    return (read() >> pinNumber) & 0x01;
}
