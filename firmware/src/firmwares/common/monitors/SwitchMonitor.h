#pragma once

#include <memory>
#include "../bitreaders/BitReader.h"

enum class SwitchState {
    PRESSED,
    UNPRESSED,
    UNKNOWN,
};

class SwitchMonitor {
public:
    SwitchMonitor(std::uint8_t switchNumber, const std::shared_ptr<BitReader>& bitReader);
    SwitchMonitor(const SwitchMonitor&) = delete;
    ~SwitchMonitor();

    void begin();
    void update();

    std::uint8_t getSwitchNumber() const;
    SwitchState getCurrentState() const;
    SwitchState getPreviousState() const;

private:
    SwitchState readState() const;

    const std::uint8_t switchNumber;
    const std::shared_ptr<BitReader> bitReader;

    SwitchState currentState = SwitchState::UNKNOWN;
    SwitchState previousState = SwitchState::UNKNOWN;
};