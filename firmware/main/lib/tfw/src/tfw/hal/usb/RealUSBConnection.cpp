#include "RealUSBConnection.h"

#include "USB.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <USB.h>

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("RealUSBConnection");

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

tfw::hal::usb::RealConnection::RealConnection(time::Clock& clock)
    : clock(clock),
      actionSender(hostComputer, clock) {
}

void tfw::hal::usb::RealConnection::setup() {
    USB.onEvent(usbEventCallback);

    USB.productName("tommy-product");
    USB.manufacturerName("tommy-manufacturer");

    USB.begin();
    hostComputer.begin();

    clock.delayMs(500);

    hostComputer.releaseAllKeys();
}

void tfw::hal::usb::RealConnection::update() {
    actionSender.update();
}

void tfw::hal::usb::RealConnection::setTypingDelayMs(const uint32_t delayMs) {
    actionSender.setTypingDelayMs(delayMs);
}

void tfw::hal::usb::RealConnection::setModifierHoldMs(const uint32_t holdMs) {
    actionSender.setModifierHoldMs(holdMs);
}

bool tfw::hal::usb::RealConnection::isBusy() {
    return actionSender.isBusy();
}

bool tfw::hal::usb::RealConnection::isConnected() {
    return ::isConnected;
}

void tfw::hal::usb::RealConnection::sendAction(Action& action) {
    actionSender.send(action);
}

#endif
