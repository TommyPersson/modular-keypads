#pragma once

#include <tfw/hal/uart.h>
#include <tfw/hal/metrics.h>
#include <tfw/hal/usb.h>

#include "firmwares/common/notifications/NotifierFactory.h"
#include "mkp/devices/common/DeviceConfigurationManager.h"
#include "firmwares/common/FirmwareModeDetector.h"
#include <tfw/hal/i2c.h>

struct ServiceLocator {
    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
    tfw::hal::uart::SerialPort& serialPort;
    NotifierFactory& notifierFactory;
    tfw::hal::i2c::Client& i2cClient;
    tfw::hal::i2c::SlavePort& i2cSlavePort;
    tfw::hal::usb::Connection& usbConnection;
    tfw::hal::metrics::MetricRegistry& metricRegistry;
    devices::FirmwareModeDetector& deviceModeDetector;
};
