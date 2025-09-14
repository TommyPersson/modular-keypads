#include "KeyBindingAction.h"

#include "MacroPadHardware.h"

std::shared_ptr<KeyBindingAction> KeyBindingAction::consumerControl(const int usageId) {
    return std::make_shared<ConsumerControlKeyBindingAction>(usageId);
}

std::shared_ptr<KeyBindingAction> KeyBindingAction::systemControl(const int usageId) {
    return std::make_shared<SystemControlKeyBindingAction>(usageId);
}

std::shared_ptr<KeyBindingAction> KeyBindingAction::keyboard(const std::vector<int>& hidKeyCodes) {
    return std::make_shared<KeyboardKeyBindingAction>(hidKeyCodes);
}

ConsumerControlKeyBindingAction::ConsumerControlKeyBindingAction(const int usageId) :
    usageId(usageId) {
}

void ConsumerControlKeyBindingAction::onKeyPressed(const MacroPadHardware& hardware) {
    hardware.serial->stream().printf("attempt cc\n");
    hardware.consumerControl.press(this->usageId);
    hardware.consumerControl.release();
}

void ConsumerControlKeyBindingAction::onKeyReleased(const MacroPadHardware& hardware) {
}

SystemControlKeyBindingAction::SystemControlKeyBindingAction(const int usageId) :
    usageId(usageId) {
}

void SystemControlKeyBindingAction::onKeyPressed(const MacroPadHardware& hardware) {
    hardware.systemControl.press(this->usageId);
    hardware.systemControl.release();
}

void SystemControlKeyBindingAction::onKeyReleased(const MacroPadHardware& hardware) {
}

KeyboardKeyBindingAction::KeyboardKeyBindingAction(const std::vector<int>& hidKeyCodes) :
    hidKeyCodes(hidKeyCodes) {
}

void KeyboardKeyBindingAction::onKeyPressed(const MacroPadHardware& hardware) {
    for (const auto& keyCode : this->hidKeyCodes) {
        hardware.keyboard.pressRaw(keyCode);
    }
}

void KeyboardKeyBindingAction::onKeyReleased(const MacroPadHardware& hardware) {
    for (int i = this->hidKeyCodes.size() - 1; i >= 0; i--) {
        const int keyCode = this->hidKeyCodes[i];
        hardware.keyboard.releaseRaw(keyCode);
    }
}
