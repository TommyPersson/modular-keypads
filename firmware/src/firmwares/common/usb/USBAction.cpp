#include "USBAction.h"

using namespace usb;

std::shared_ptr<Action> Action::keyPress(const std::vector<uint8_t>& keyCodes) {
    return std::make_shared<KeyPressAction>(KeyPressData{ .keyCodes = keyCodes });
}
