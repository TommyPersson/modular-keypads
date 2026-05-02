#pragma once

#include <../../../lib/tfw/src/tfw/hal/gpio/PhysicalInputPin.h>

#include "DeviceMode.h"

namespace devices {
    class DeviceModeDetector {
    public:
        void setup();

        DeviceMode detectDeviceMode() const;
    private:
        std::unique_ptr<tfw::hal::gpio::InputPin> vbusPin;
    };
}
