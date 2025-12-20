#include "USBAction.h"

using namespace usb;

std::shared_ptr<Action> Action::keyPress(uint8_t keyCode) {
    return std::make_shared<KeyPressAction>(KeyPressData{ .keyCode = keyCode });
}
