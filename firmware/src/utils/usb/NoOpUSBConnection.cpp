#include "NoOpUSBConnection.h"

void utils::usb::NoOpConnection::setup() {
}

bool utils::usb::NoOpConnection::isConnected() {
    return false;
}

void utils::usb::NoOpConnection::sendAction(Action& action) {
}
