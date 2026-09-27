#pragma once
#include "hal/base/Component.h"

namespace smc::firmwares {
    class Firmware : public smc::hal::Component {
    public:
        ~Firmware() override = default;

        void setup() override {}
        void update(uint32_t deltaUs, smc::events::EventQueue& queue) override {}
    };
}