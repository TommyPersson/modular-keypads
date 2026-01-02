#include "NoOpUSBConnection.h"

void usb::NoOpConnection::setup() {
}

bool usb::NoOpConnection::isConnected() {
    return false;
}

void usb::NoOpConnection::sendAction(Action& action) {
}
