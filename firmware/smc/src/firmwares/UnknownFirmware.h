#pragma once
#include "Firmware.h"

namespace smc::firmwares {
    class UnknownFirmware : public Firmware {
    public:
        explicit UnknownFirmware(uint8_t smcType);
        UnknownFirmware() = default;

        void setup() override;

    private:
        int smcType{};
    };
}
