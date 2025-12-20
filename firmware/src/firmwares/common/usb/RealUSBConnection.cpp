#include "RealUSBConnection.h"

#include <USB.h>

#ifdef SOC_USB_OTG_SUPPORTED

#include <esp32-hal.h>
#include <USBHIDKeyboard.h>
#include <USBHIDConsumerControl.h>
#include <USBHIDSystemControl.h>
#include <firmwares/common/logging/Logger.h>

namespace {
    auto logger = common::logging::createLogger("RealUSBConnection");

    USBHIDKeyboard keyboard;
    USBHIDConsumerControl consumerControl;
    USBHIDSystemControl systemControl;

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
            switch (event_id) {
            case ARDUINO_USB_RESUME_EVENT:
                logger->debug("ARDUINO_USB_RESUME_EVENT");
                break;
            case ARDUINO_USB_STARTED_EVENT:
                logger->debug("ARDUINO_USB_STARTED_EVENT");
                break;
            case ARDUINO_USB_STOPPED_EVENT:
                logger->debug("ARDUINO_USB_STOPPED_EVENT");
                break;
            case ARDUINO_USB_SUSPEND_EVENT:
                logger->debug("ARDUINO_USB_SUSPEND_EVENT");
                break;
            default:
                break;
            }
        }
    }
}

void usb::RealConnection::setup() {
    //USB.onEvent(usbEventCallback);

    USB.productName("tommy-product");
    USB.manufacturerName("tommy-manufacturer");

    USB.begin();
    keyboard.begin();
    consumerControl.begin();
    systemControl.begin();

    delay(500);

    keyboard.releaseAll();

}

bool usb::RealConnection::isConnected() {
    return ::isConnected;
}

void usb::RealConnection::sendAction(Action& action) {
    const auto keyPressAction = dynamic_cast<KeyPressAction*>(&action);
    if (keyPressAction != nullptr) {
        auto keyCode = keyPressAction->data.keyCode;
        logger->debug("Sending key press: %02x", keyCode);
        keyboard.pressRaw(keyCode);
        keyboard.releaseAll();
    }
}

#endif