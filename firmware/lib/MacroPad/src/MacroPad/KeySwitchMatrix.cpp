#include "KeySwitchMatrix.h"

#include "HardwareSerial.h"

KeySwitchMatrix::KeySwitchMatrix(
    const std::vector<uint8_t>& rowPins,
    const std::vector<uint8_t>& colPins
    ) {

    for (const auto& pin : rowPins) {
        this->rowPins.emplace_back(InputPin::physical(pin, PULLDOWN));
    }

    for (const auto& pin : colPins) {
        this->colPins.emplace_back(pin);
    }

    for (int rowPinIndex = 0; rowPinIndex < this->rowPins.size(); rowPinIndex++) {
        for (int colPinIndex = 0; colPinIndex < this->colPins.size(); colPinIndex++) {
            auto keySwitch = KeySwitch{
                .rowPin = *this->rowPins[rowPinIndex],
                .colPin = this->colPins[colPinIndex],
                .row = rowPinIndex,
                .col = colPinIndex
            };

            this->keySwitches.push_back(keySwitch);
            this->lastKeyStates.push_back(false);
        }
    }
}


KeySwitchMatrix::~KeySwitchMatrix() {
}

const std::vector<KeySwitch>& KeySwitchMatrix::getKeySwitches() const {
    return this->keySwitches;
}

void KeySwitchMatrix::begin() {
    for (const auto& pin : this->rowPins) {
        pin->init();
    }

    for (const auto& pin : this->colPins) {
        pin.init();
        pin.setLow();
    }
}

MatrixScanResult KeySwitchMatrix::scan() {
    std::vector<const KeySwitch*> pressedKeys;
    std::vector<const KeySwitch*> releasedKeys;
    std::vector<const KeySwitch*> heldKeys;

    for (int i = 0; i < this->keySwitches.size(); i++) {
        const auto& keySwitch = this->keySwitches[i];

        keySwitch.colPin.setHigh();
        const bool isPressed = keySwitch.rowPin.read() == 1;
        keySwitch.colPin.setLow();

        if (isPressed) {
            heldKeys.push_back(&keySwitch);
        }

        if (isPressed != this->lastKeyStates[i]) {
            if (isPressed) {
                pressedKeys.push_back(&keySwitch);
            } else {
                releasedKeys.push_back(&keySwitch);
            }
        }

        this->lastKeyStates[i] = isPressed;
    }

    return MatrixScanResult{
        .pressedKeys = pressedKeys,
        .releasedKeys = releasedKeys,
        .heldKeys = heldKeys,
    };
}
