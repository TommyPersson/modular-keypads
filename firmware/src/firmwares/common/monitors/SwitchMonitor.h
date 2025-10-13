#pragma once

#include <memory>
#include "../bitreaders/BitReader.h"

class SwitchMonitor {
public:
    SwitchMonitor(std::uint8_t switchNumber, const std::shared_ptr<BitReader>& bitReader);
    SwitchMonitor(const SwitchMonitor&) = delete;
    ~SwitchMonitor();

    void begin();
    void update();

    std::uint8_t getSwitchNumber() const;
    bool getCurrentState() const;
    bool getPreviousState() const;

private:
    const std::uint8_t switchNumber;
    const std::shared_ptr<BitReader> bitReader;

    bool currentState = false;
    bool previousState = false;
};