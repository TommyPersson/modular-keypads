#pragma once

#include <tfw/hal/time/Clock.h>
#include <tfw/hal/usb/USBConnection.h>

#include "FirmwareMode.h"

namespace mkp::firmwares::base {
    class FirmwareModeDetector {
    public:
        FirmwareModeDetector(tfw::hal::usb::Connection& usbConnection, tfw::hal::time::Clock& clock);

        FirmwareMode detectFirmwareMode() const;

        // Restarts the device and makes the next detection pick the master without waiting for the host, so that a
        // host that is always slower than the enumeration timeout doesn't make the device restart over and over
        [[noreturn]] void restartAsMaster();

    private:
        tfw::hal::usb::Connection& usbConnection;
        tfw::hal::time::Clock& clock;
    };
}
