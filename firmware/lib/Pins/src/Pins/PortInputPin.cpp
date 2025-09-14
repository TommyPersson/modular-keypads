#include "PortInputPin.h"

PortInputPin::PortInputPin(std::uint8_t pinNumber, const ReadPort& port) :
    InputPin(pinNumber),
    port(port) {
}

void PortInputPin::init() const {
}

std::uint8_t PortInputPin::read() const {
    return (port.read() >> pinNumber) & 0x01;
}
