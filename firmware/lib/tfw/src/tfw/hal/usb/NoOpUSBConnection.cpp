#include "NoOpUSBConnection.h"

void tfw::hal::usb::NoOpConnection::setup() {
}

void tfw::hal::usb::NoOpConnection::update() {
}

bool tfw::hal::usb::NoOpConnection::isConnected() {
    return false;
}

void tfw::hal::usb::NoOpConnection::sendAction(Action& action) {
}
