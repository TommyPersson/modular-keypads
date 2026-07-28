#pragma once

#include <tfw/ic/L74165.h>

namespace devices {
    class DeviceTypeDetector {
    public:
        DeviceTypeDetector();

        void setup() const;

        char detectDeviceType() const;

    private:
        std::unique_ptr<tfw::ic::L74165> shiftRegister;
    };
}
