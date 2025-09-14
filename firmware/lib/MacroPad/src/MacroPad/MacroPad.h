#pragma once

#include <cstdint>

#include "KeySwitchMatrix.h"
#include "KeyBinding.h"
#include "KeyMap.h"

class MacroPad {
public:
    MacroPad(
        const std::vector<std::uint8_t>& rowPins,
        const std::vector<std::uint8_t>& colPins,
        const MacroPadHardware& hardware);

    void begin() const;

    void setBindings(const std::vector<KeyBinding>& bindings);

    void tick();

private:
    void handleKeyPress(const KeySwitch& keySwitch) const;
    void handleKeyRelease(const KeySwitch& keySwitch) const;
    void handleKeyMatrixScanResult(const MatrixScanResult& scanResult) const;

    std::unique_ptr<KeySwitchMatrix> matrix;
    std::unique_ptr<KeyMap> keyMap;

    const MacroPadHardware& hardware;
};
