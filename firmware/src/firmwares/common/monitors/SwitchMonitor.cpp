#include "SwitchMonitor.h"

SwitchMonitor::SwitchMonitor(
    const std::uint8_t switchNumber,
    const std::shared_ptr<BitReader>& bitReader
    ) :
    switchNumber(switchNumber),
    bitReader(bitReader) {
}

SwitchMonitor::~SwitchMonitor() = default;

void SwitchMonitor::begin() {
    previousState = readState();
    currentState = previousState;
}

void SwitchMonitor::update() {
    previousState = currentState;
    currentState = readState();

    // TODO some form of observable
}

std::uint8_t SwitchMonitor::getSwitchNumber() const {
    return switchNumber;
}

SwitchState SwitchMonitor::getCurrentState() const {
    return currentState;
}

SwitchState SwitchMonitor::getPreviousState() const {
    return previousState;
}

SwitchState SwitchMonitor::readState() const {
    return bitReader->read() ? SwitchState::PRESSED : SwitchState::UNPRESSED;
}
