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
    if (now - lastKeyboardWriteTime >= typingDelayUs) { // Without a delay then repeated characters can be missed.
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

bool tfw::hal::usb::RealConnection::isConnected() {
    return ::isConnected;
}

void tfw::hal::usb::RealConnection::sendAction(Action& action) {
    const auto keyPressAction = dynamic_cast<KeyPressAction*>(&action);
    if (keyPressAction != nullptr) {
        for (auto keyCode : keyPressAction->data.keyCodes) {
            keyboard.pressRaw(keyCode);
        }

        keyboard.releaseAll();
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

    const auto mouseAction = dynamic_cast<MouseAction*>(&action);
    if (mouseAction != nullptr) {
        if (mouseAction->buttons != 0) {
            mouse.click(mouseAction->buttons);
        }

        if (mouseAction->x != 0 || mouseAction->y != 0 || mouseAction->wheel != 0 || mouseAction->pan != 0) {
            mouse.move(mouseAction->x, mouseAction->y, mouseAction->wheel, mouseAction->pan);
        }
    }
}

#endif
