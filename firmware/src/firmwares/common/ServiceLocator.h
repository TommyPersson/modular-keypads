#pragma once

#include <Wire.h>

#include <SerialPort/SerialPort.h>
#include "firmwares/common/notifications/NotifierFactory.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "i2c/I2cClient.h"
#include "i2c/SlavePort.h"

struct ServiceLocator {
    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    NotifierFactory& notifierFactory;
    I2cClient& i2cClient;
    i2c::SlavePort& i2cSlavePort;
    TwoWire& i2c;
};
