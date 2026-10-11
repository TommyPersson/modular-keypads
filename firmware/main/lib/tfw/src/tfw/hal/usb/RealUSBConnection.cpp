#include "RealUSBConnection.h"

#include "USB.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <USB.h>
#include <USBHIDConsumerControl.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>
#include <USBHIDSystemControl.h>

#include <tfw/hal/logging.h>
#include <tfw/hal/time.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("RealUSBConnection");

    USBHIDKeyboard keyboard;
    USBHIDConsumerControl consumerControl;
    USBHIDSystemControl systemControl;
    USBHIDMouse mouse;

    bool isConnected = false;

    void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
        if (event_base == ARDUINO_USB_EVENTS) {
            switch (event_id) {
            case ARDUINO_USB_RESUME_EVENT:
            case ARDUINO_USB_STARTED_EVENT:
                isConnected = true;
                break;
            case ARDUINO_USB_STOPPED_EVENT:
            case ARDUINO_USB_SUSPEND_EVENT:
                isConnected = false;
                break;
            default:
                break;
            }
        }
    }
}

void tfw::hal::usb::RealConnection::setup() {
    USB.onEvent(usbEventCallback);

    USB.productName("tommy-product");
    USB.manufacturerName("tommy-manufacturer");

    USB.begin();
    keyboard.begin();
    consumerControl.begin();
    systemControl.begin();
    mouse.begin();

    time::delayMs(500);

    keyboard.releaseAll();
}

void tfw::hal::usb::RealConnection::update() {
    const auto now = time::micros();

    applyModifierChange(modifierHolder.update(now));

    const auto enoughTimePassSinceLastWrite = now - lastKeyboardWriteTime >= typingDelayUs;
    const auto canWrite = enoughTimePassSinceLastWrite && !modifierHolder.isHeld();
    if (canWrite) {
        const auto nextChar = characterOutputQueue.dequeue();
        if (nextChar != nullptr) {
            keyboard.write(*nextChar);

            // Only updated on writes, so that the first character of a text isn't delayed
            lastKeyboardWriteTime = now;
        }
    }
}

void tfw::hal::usb::RealConnection::setTypingDelayMs(const uint32_t delayMs) {
    typingDelayUs = static_cast<uint64_t>(delayMs) * 1'000;
}

void tfw::hal::usb::RealConnection::setModifierHoldMs(const uint32_t holdMs) {
    modifierHolder.setDefaultHoldTimeUs(static_cast<uint64_t>(holdMs) * 1'000);
}

void tfw::hal::usb::RealConnection::applyModifierChange(const ModifierChange& change) {
    // Modifier key codes 0xe0 to 0xe7 are in the same order as the bits of the modifier flags
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (change.release & (1 << bit)) {
            keyboard.releaseRaw(0xe0 + bit);
        }
    }

    for (uint8_t bit = 0; bit < 8; bit++) {
        if (change.press & (1 << bit)) {
            keyboard.pressRaw(0xe0 + bit);
        }
    }
}

bool tfw::hal::usb::RealConnection::isConnected() {
    return ::isConnected;
}

void tfw::hal::usb::RealConnection::sendAction(Action& action) {
    const auto shortcutAction = dynamic_cast<ShortcutAction*>(&action);
    const auto mouseAction = dynamic_cast<MouseAction*>(&action);

    // Other actions have no use for held modifiers
    if (shortcutAction == nullptr && mouseAction == nullptr) {
        applyModifierChange(modifierHolder.releaseAll());
    }

    if (shortcutAction != nullptr) {
        const auto holdTimeUs = shortcutAction->holdTimeMs > 0
            ? std::optional(static_cast<uint64_t>(shortcutAction->holdTimeMs) * 1'000)
            : std::nullopt;
        applyModifierChange(modifierHolder.hold(shortcutAction->modifiers, time::micros(), holdTimeUs));

        if (shortcutAction->keyCode != 0) {
            keyboard.pressRaw(shortcutAction->keyCode);
            keyboard.releaseRaw(shortcutAction->keyCode);
        }

        if (!shortcutAction->holdModifiers) {
            applyModifierChange(modifierHolder.releaseAll());
        }
    }

    const auto consumerControlAction = dynamic_cast<ConsumerControlAction*>(&action);
    if (consumerControlAction != nullptr) {
        consumerControl.press(consumerControlAction->usageId);
        consumerControl.release();
    }

    const auto systemControlAction = dynamic_cast<SystemControlAction*>(&action);
    if (systemControlAction != nullptr) {
        systemControl.press(systemControlAction->code);
        systemControl.release();
    }

    const auto typeAction = dynamic_cast<TypeAction*>(&action);
    if (typeAction != nullptr) {
        for (auto& character : typeAction->text) {
            characterOutputQueue.enqueue(character);
        }
    }

    if (mouseAction != nullptr) {
        applyModifierChange(modifierHolder.hold(mouseAction->modifiers, time::micros()));

        if (mouseAction->buttons != 0) {
            mouse.click(mouseAction->buttons);
        }

        if (mouseAction->x != 0 || mouseAction->y != 0 || mouseAction->wheel != 0 || mouseAction->pan != 0) {
            mouse.move(mouseAction->x, mouseAction->y, mouseAction->wheel, mouseAction->pan);
        }
    }
}

#endif
