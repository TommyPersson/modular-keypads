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
    previousState = bitReader->read();
    currentState = previousState;
}

void SwitchMonitor::update() {
    previousState = currentState;
    currentState = bitReader->read();

    // TODO some form of observable
}

std::uint8_t SwitchMonitor::getSwitchNumber() const {
    return switchNumber;
}

bool SwitchMonitor::getCurrentState() const {
    return currentState;
}

bool SwitchMonitor::getPreviousState() const {
    return previousState;
}
