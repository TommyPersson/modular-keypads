#pragma once
#include "Firmware.h"

namespace smc::firmwares {
    class UnknownFirmware : public Firmware {
    public:
        explicit UnknownFirmware(uint8_t smcType);
        ~UnknownFirmware() override = default;

        uint8_t deviceType() override { return smcType; };

    protected:
        void doSetup() override;

    private:
        int smcType{};
    };
}
