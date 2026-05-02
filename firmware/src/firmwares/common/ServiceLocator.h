#pragma once

#include <tfw/hal/uart.h>
#include <tfw/hal/metrics.h>
#include <tfw/hal/usb.h>

#include "firmwares/common/notifications/NotifierFactory.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/DeviceModeDetector.h"
#include <tfw/hal/i2c.h>
#include <tfw/hal/i2c.h>

struct ServiceLocator {
    DeviceConfigurationManager& deviceConfigurationManager;
    tfw::hal::uart::SerialPort& serialPort;
    NotifierFactory& notifierFactory;
    tfw::hal::i2c::Client& i2cClient;
    tfw::hal::i2c::SlavePort& i2cSlavePort;
    tfw::hal::usb::Connection& usbConnection;
    tfw::hal::metrics::MetricRegistry& metricRegistry;
    devices::DeviceModeDetector& deviceModeDetector;
};
