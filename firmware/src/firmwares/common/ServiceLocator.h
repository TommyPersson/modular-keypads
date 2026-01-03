#pragma once

#include <Wire.h>

#include <SerialPort/SerialPort.h>
#include "firmwares/common/notifications/NotifierFactory.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "utils/i2c/Client.h"
#include "utils/i2c/SlavePort.h"
#include "utils/usb/USBConnection.h"

struct ServiceLocator {
    DeviceConfigurationManager& deviceConfigurationManager;
    utils::serial::SerialPort& serialPort;
    NotifierFactory& notifierFactory;
    utils::i2c::Client& i2cClient;
    utils::i2c::SlavePort& i2cSlavePort;
    TwoWire& i2c;
    utils::usb::Connection& usbConnection;
};
