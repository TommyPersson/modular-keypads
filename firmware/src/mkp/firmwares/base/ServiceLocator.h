#pragma once

#include <tfw/hal/metrics.h>
#include <tfw/hal/uart.h>
#include <tfw/hal/usb.h>
#include <tfw/hal/i2c.h>
#include <tfw/hal/time.h>
#include <tfw/hal/fs.h>

#include "FirmwareModeDetector.h"
#include "mkp/components/notifications/NotifierFactory.h"
#include "mkp/devices/common/DeviceConfigurationManager.h"

namespace mkp::firmwares::base {
    struct ServiceLocator {
        devices::common::DeviceConfigurationManager& deviceConfigurationManager;
        tfw::hal::uart::SerialPort& serialPort;
        components::notifications::NotifierFactory& notifierFactory;
        tfw::hal::i2c::Client& i2cClient;
        tfw::hal::i2c::SlavePort& i2cSlavePort;
        tfw::hal::usb::Connection& usbConnection;
        tfw::hal::metrics::MetricRegistry& metricRegistry;
        FirmwareModeDetector& deviceModeDetector;
        tfw::hal::time::Clock& clock;
        tfw::hal::fs::FileSystem& fileSystem;
    };
}
