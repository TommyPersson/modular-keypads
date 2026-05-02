#include "NoOpUSBConnection.h"

void tfw::utils::usb::NoOpConnection::setup() {
}

bool tfw::utils::usb::NoOpConnection::isConnected() {
    return false;
}

void tfw::utils::usb::NoOpConnection::sendAction(Action& action) {
}
