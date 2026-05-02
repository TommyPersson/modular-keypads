#pragma once

#include <memory>

#include <tfw/hal/bitreaders.h>
#include <tfw/utils/observables.h>

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
    SwitchMonitor(std::uint8_t switchNumber, const std::shared_ptr<tfw::hal::bitreaders::BitReader>& bitReader);
    SwitchMonitor(const SwitchMonitor&) = delete;
    ~SwitchMonitor();

    void begin();
    void update();

    tfw::utils::observables::Observable<SwitchEvent>& onSwitchStateChanged() const;

    std::uint8_t getSwitchNumber() const;
    SwitchState getCurrentState() const;
    SwitchState getPreviousState() const;

private:
    SwitchState readState() const;

    const std::uint8_t switchNumber;
    const std::shared_ptr<tfw::hal::bitreaders::BitReader> bitReader;

    std::unique_ptr<tfw::utils::observables::Subject<SwitchEvent>> keySwitchStateChangedSubject;

    SwitchState currentState = SwitchState::UNKNOWN;
    SwitchState previousState = SwitchState::UNKNOWN;
};