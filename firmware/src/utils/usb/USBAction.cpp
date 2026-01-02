#include "USBAction.h"

using namespace utils::usb;

std::shared_ptr<Action> Action::keyPress(const std::vector<uint8_t>& keyCodes) {
    return std::make_shared<KeyPressAction>(KeyPressData{ .keyCodes = keyCodes });
}

std::shared_ptr<Action> Action::consumerControl(uint16_t usageId) {
    return std::make_shared<ConsumerControlAction>(usageId);
}

std::shared_ptr<Action> Action::systemControl(uint8_t code) {
    return std::make_shared<SystemControlAction>(code);
}
