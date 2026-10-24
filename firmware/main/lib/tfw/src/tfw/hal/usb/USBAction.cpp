#include "USBAction.h"

using namespace tfw::hal::usb;

std::shared_ptr<Action> Action::shortcut(
    const uint8_t modifiers,
    const uint8_t keyCode,
    const bool holdModifiers,
    const uint32_t holdTimeMs
) {
    return std::make_shared<ShortcutAction>(modifiers, keyCode, holdModifiers, holdTimeMs);
}

std::shared_ptr<Action> Action::consumerControl(uint16_t usageId) {
    return std::make_shared<ConsumerControlAction>(usageId);
}

std::shared_ptr<Action> Action::systemControl(uint8_t code) {
    return std::make_shared<SystemControlAction>(code);
}

std::shared_ptr<Action> Action::type(const std::string_view& text) {
    return std::make_shared<TypeAction>(text);
}

std::shared_ptr<Action> Action::mouse(
    uint8_t modifiers,
    uint8_t buttons,
    int16_t x,
    int16_t y,
    int8_t wheel,
    int8_t pan
) {
    return std::make_shared<MouseAction>(modifiers, buttons, x, y, wheel, pan);
}
