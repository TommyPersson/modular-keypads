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
    tfw::utils::serial::SerialPort& serialPort;
    NotifierFactory& notifierFactory;
    tfw::utils::i2c::Client& i2cClient;
    tfw::utils::i2c::SlavePort& i2cSlavePort;
    tfw::utils::usb::Connection& usbConnection;
    tfw::utils::metrics::MetricRegistry& metricRegistry;
    devices::DeviceModeDetector& deviceModeDetector;
};
