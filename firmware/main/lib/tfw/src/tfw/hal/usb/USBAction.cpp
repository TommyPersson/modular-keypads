#include "USBAction.h"

using namespace tfw::hal::usb;

std::shared_ptr<Action> Action::keyPress(const std::vector<uint8_t>& keyCodes) {
    return std::make_shared<KeyPressAction>(KeyPressData{ .keyCodes = keyCodes });
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
    int8_t x,
    int8_t y,
    int8_t wheel,
    int8_t pan
) {
    return std::make_shared<MouseAction>(modifiers, buttons, x, y, wheel, pan);
}
