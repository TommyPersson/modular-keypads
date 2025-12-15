#pragma once

#include <Wire.h>

#include <SerialPort/SerialPort.h>
#include "firmwares/common/notifications/Notifier.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/logging/Logger.h"
#include "i2c/I2cClient.h"
#include "i2c/SlavePort.h"

struct ServiceLocator {
    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    Notifier& notifier;
    I2cClient& i2cClient;
    i2c::SlavePort& i2cSlavePort;
    TwoWire& i2c;
};
