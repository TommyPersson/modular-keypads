#include "SwitchMonitor.h"

SwitchMonitor::SwitchMonitor(
    const std::uint8_t switchNumber,
    const std::shared_ptr<BitReader>& bitReader
    ) :
    switchNumber(switchNumber),
    bitReader(bitReader),
    keySwitchStateChangedSubject(std::make_unique<utils::observables::Subject<SwitchEvent>>()) {
}

SwitchMonitor::~SwitchMonitor() = default;

void SwitchMonitor::begin() {
    previousState = readState();
    currentState = previousState;
}

void SwitchMonitor::update() {
    previousState = currentState;
    currentState = readState();

    if (currentState != previousState) {
        keySwitchStateChangedSubject->notify({.switchNumber = this->switchNumber, .state = currentState});
    }
}

utils::observables::Observable<SwitchEvent>& SwitchMonitor::onSwitchStateChanged() const {
    return *keySwitchStateChangedSubject;
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
