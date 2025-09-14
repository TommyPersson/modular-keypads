#pragma once

#include <cstdint>
#include <vector>

#include "KeySwitch.h"
#include "Pins/InputPin.h"
#include "Pins/OutputPin.h"

struct MatrixScanResult {
    const std::vector<const KeySwitch*> pressedKeys;
    const std::vector<const KeySwitch*> releasedKeys;
    const std::vector<const KeySwitch*> heldKeys;
};

class KeySwitchMatrix {

public:
    KeySwitchMatrix(const std::vector<uint8_t>& rowPins, const std::vector<uint8_t>& colPins);
    KeySwitchMatrix(const KeySwitchMatrix&) = delete;
    KeySwitchMatrix& operator=(KeySwitchMatrix const&) = delete;
    ~KeySwitchMatrix();

    void begin();

    MatrixScanResult scan();

    const std::vector<KeySwitch>& getKeySwitches() const;

private:
    std::vector<std::unique_ptr<InputPin>> rowPins;
    std::vector<OutputPin> colPins;
    std::vector<KeySwitch> keySwitches;

    std::vector<bool> lastKeyStates;
};
