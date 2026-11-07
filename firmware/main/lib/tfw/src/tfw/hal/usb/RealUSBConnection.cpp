#include "RealUSBConnection.h"

#include "USB.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <USB.h>

#include <atomic>

#include <soc/soc.h>
#include <soc/usb_serial_jtag_reg.h>

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("RealUSBConnection");

    // Below this, nothing powers VBUS
    constexpr uint16_t VBUS_PRESENT_THRESHOLD = 500;

    // How long VBUS takes to discharge through the sense divider once the D+ pull-up stops leaking onto it
    constexpr uint32_t VBUS_SETTLE_DELAY_MS = 10;

    // Written by the USB event task, read by the main loop
    std::atomic<bool> isConnected = false;

    // The ROM leaves the PHY with the USB Serial/JTAG controller and its D+ pull-up enabled. The pull-up leaks onto
    // VBUS through the ESD protection, so it's overridden off until a host may be present. Releasing the override
    // gives the pull-up back to the controller, before USB.begin() hands the PHY to the OTG controller.
    void enableDPlusPullUp() {
        REG_CLR_BIT(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP | USB_SERIAL_JTAG_DP_PULLDOWN);
        REG_SET_BIT(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
    }

    void disableDPlusPullUp() {
        REG_CLR_BIT(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
    }

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

tfw::hal::usb::RealConnection::RealConnection(time::Clock& clock, std::unique_ptr<gpio::InputPin> vbusPin)
    : clock(clock),
      vbusPin(std::move(vbusPin)),
      actionSender(hostComputer, clock) {
}

void tfw::hal::usb::RealConnection::setup() {
    disableDPlusPullUp();
    clock.delayMs(VBUS_SETTLE_DELAY_MS);

    if (!isVbusPresent()) {
        logger->info("No VBUS, leaving USB off");
        return;
    }

    enableDPlusPullUp();

    USB.onEvent(usbEventCallback);

    USB.productName("tommy-product");
    USB.manufacturerName("tommy-manufacturer");

    started = USB.begin();
    if (!started) {
        logger->error("Unable to start USB");
        return;
    }

    hostComputer.begin();

    clock.delayMs(500);

    hostComputer.releaseAllKeys();
}

void tfw::hal::usb::RealConnection::update() {
    if (started) {
        actionSender.update();
    }
}

void tfw::hal::usb::RealConnection::setTypingDelayMs(const uint32_t delayMs) {
    actionSender.setTypingDelayMs(delayMs);
}

void tfw::hal::usb::RealConnection::setModifierHoldMs(const uint32_t holdMs) {
    actionSender.setModifierHoldMs(holdMs);
}

bool tfw::hal::usb::RealConnection::isBusy() {
    return started && actionSender.isBusy();
}

bool tfw::hal::usb::RealConnection::isStarted() {
    return started;
}

bool tfw::hal::usb::RealConnection::isConnected() {
    return ::isConnected;
}

bool tfw::hal::usb::RealConnection::isVbusPresent() const {
    // Only reliable while the D+ pull-up is forced off, as it leaks onto VBUS through the ESD protection
    return vbusPin->readAnalog() >= VBUS_PRESENT_THRESHOLD;
}

void tfw::hal::usb::RealConnection::sendAction(Action& action) {
    if (started) {
        actionSender.send(action);
    }
}

#endif
