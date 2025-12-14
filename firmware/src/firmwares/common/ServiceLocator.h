#pragma once

#include <Wire.h>

#include <SerialPort/SerialPort.h>
#include "firmwares/common/notifications/Notifier.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/logging/Logger.h"

struct ServiceLocator {
    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    Notifier& notifier;
    TwoWire& i2c;
};
