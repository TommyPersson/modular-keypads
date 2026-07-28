#pragma once

#include <../../../lib/tfw/src/tfw/hal/gpio/PhysicalInputPin.h>

#include "FirmwareMode.h"

namespace devices {
    class FirmwareModeDetector {
    public:
        void setup();

        FirmwareMode detectFirmwareMode() const;
    private:
        std::unique_ptr<tfw::hal::gpio::InputPin> vbusPin;
    };
}
