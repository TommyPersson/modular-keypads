#include "NoOpUSBConnection.h"

void tfw::hal::usb::NoOpConnection::setup() {
}

bool tfw::hal::usb::NoOpConnection::isConnected() {
    return false;
}

void tfw::hal::usb::NoOpConnection::sendAction(Action& action) {
}
