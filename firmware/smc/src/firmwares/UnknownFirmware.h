#pragma once
#include "Firmware.h"

namespace smc::firmwares {
    class UnknownFirmware : public Firmware {
    public:
        explicit UnknownFirmware(uint8_t smcType);


    protected:
        void doSetup() override;

    private:
        int smcType{};
    };
}
