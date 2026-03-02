#pragma once

#include <L74165/L74165.h>

namespace devices {
    class DeviceTypeDetector {
    public:
        DeviceTypeDetector();

        void setup() const;

        char detectDeviceType() const;

    private:
        std::unique_ptr<chips::L74165::L74165> shiftRegister;
    };
}
