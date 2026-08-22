#pragma once

#include <tfw/ic/L74165.h>
#include <tfw/hal/time.h>

namespace mkp::devices::common {
    class DeviceTypeDetector {
    public:
        explicit DeviceTypeDetector(tfw::hal::time::Clock& clock);

        void setup() const;

        char detectDeviceType() const;

    private:
        std::unique_ptr<tfw::ic::L74165> shiftRegister;
    };
}
