#pragma once

#include <Pins/PhysicalInputPin.h>

#include "DeviceMode.h"

namespace devices {
    class DeviceModeDetector {
    public:
        void setup();

        DeviceMode detectDeviceMode() const;
    private:
        std::unique_ptr<utils::pins::InputPin> vbusPin;
    };
}
