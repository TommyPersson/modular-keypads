#pragma once

#include "Firmware.h"
#include "ic/L74165.h"

namespace smc::firmwares {
    class FirmwareFactory {
    public:
        explicit FirmwareFactory(ic::L74165::Config typeSelectorConfig);

        Firmware* create();

    private:
        uint8_t readSmcType();

        ic::L74165::L74165 typeSelector;
    };
}
