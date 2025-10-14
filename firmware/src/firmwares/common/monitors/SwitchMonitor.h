#pragma once

#include <memory>

#include "../bitreaders/BitReader.h"
#include "utils/observables/Observable.h"
#include "utils/observables/Subject.h"

enum class SwitchState {
    PRESSED,
    UNPRESSED,
    UNKNOWN,
};

struct SwitchEvent {
    uint8_t switchNumber;
    SwitchState state;
};

class SwitchMonitor {
public:
    SwitchMonitor(std::uint8_t switchNumber, const std::shared_ptr<BitReader>& bitReader);
    SwitchMonitor(const SwitchMonitor&) = delete;
    ~SwitchMonitor();

    void begin();
    void update();

    Observable<SwitchEvent>& onSwitchStateChanged() const;

    std::uint8_t getSwitchNumber() const;
    SwitchState getCurrentState() const;
    SwitchState getPreviousState() const;

private:
    SwitchState readState() const;

    const std::uint8_t switchNumber;
    const std::shared_ptr<BitReader> bitReader;

    std::unique_ptr<Subject<SwitchEvent>> keySwitchStateChangedSubject;

    SwitchState currentState = SwitchState::UNKNOWN;
    SwitchState previousState = SwitchState::UNKNOWN;
};