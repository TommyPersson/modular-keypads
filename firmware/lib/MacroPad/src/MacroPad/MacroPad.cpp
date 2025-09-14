#include <memory>

#include <Arduino.h>
#include <USB.h>

#include "MacroPadHardware.h"

#include "MacroPad.h"


MacroPad::MacroPad(
    const std::vector<std::uint8_t>& rowPins,
    const std::vector<std::uint8_t>& colPins,
    const MacroPadHardware& hardware
    ) :
    hardware(hardware) {
    this->matrix = std::make_unique<KeySwitchMatrix>(rowPins, colPins);
}

void MacroPad::tick() {
    const auto scanResult = this->matrix->scan();
    handleKeyMatrixScanResult(scanResult);
}

void MacroPad::begin() const {
    this->hardware.indicatorLedPin.init();
    this->hardware.usb.begin();
    this->hardware.serial->begin(9600);
    this->hardware.consumerControl.begin();
    this->hardware.systemControl.begin();
    this->hardware.keyboard.begin();

    delay(500);

    this->hardware.keyboard.releaseAll();

    this->matrix->begin();
}

void MacroPad::setBindings(const std::vector<KeyBinding>& bindings) {
    this->keyMap = std::make_unique<KeyMap>(bindings);
}

void MacroPad::handleKeyPress(const KeySwitch& keySwitch) const {
    if (const auto binding = keyMap->getKeyBinding(keySwitch); binding != nullptr) {
        binding->action->onKeyPressed(this->hardware);
    }
}

void MacroPad::handleKeyRelease(const KeySwitch& keySwitch) const {
    if (const auto binding = keyMap->getKeyBinding(keySwitch); binding != nullptr) {
        binding->action->onKeyReleased(this->hardware);
    }
}

void MacroPad::handleKeyMatrixScanResult(const MatrixScanResult& scanResult) const {
    for (const auto& keySwitch : scanResult.pressedKeys) {
        handleKeyPress(*keySwitch);
    }

    for (const auto& keySwitch : scanResult.releasedKeys) {
        handleKeyRelease(*keySwitch);
    }

    this->hardware.keyboard.flush();

    for (const auto& keySwitch : scanResult.pressedKeys) {
        this->hardware.serial->stream().printf("Pressed:  %i%i\n", keySwitch->row, keySwitch->col);
    }

    for (const auto& keySwitch : scanResult.releasedKeys) {
        this->hardware.serial->stream().printf("Released: %i%i\n", keySwitch->row, keySwitch->col);
    }

    this->hardware.serial->stream().flush();

    if (!scanResult.heldKeys.empty()) {
        this->hardware.indicatorLedPin.setHigh();
    } else {
        this->hardware.indicatorLedPin.setLow();
    }
}
