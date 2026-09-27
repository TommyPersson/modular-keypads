#pragma once

#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/buttons/PushButton.h"

namespace smc::firmwares {
    class Smc2Firmware : public Firmware {
    public:
        Smc2Firmware();
        ~Smc2Firmware() override;
        void setup() override;
        void update(uint32_t deltaUs, smc::events::EventQueue& queue) override;

    private:
        smc::hal::buttons::PushButton buttons[6] = {
            smc::hal::buttons::PushButton(1, PIN_PA3),
            smc::hal::buttons::PushButton(2, PIN_PA4),
            smc::hal::buttons::PushButton(3, PIN_PA5),
            smc::hal::buttons::PushButton(4, PIN_PB0),
            smc::hal::buttons::PushButton(5, PIN_PB1),
            smc::hal::buttons::PushButton(6, PIN_PB2),
        };

        uint8_t buttonLedMap[6] = {0, 1, 2, 5, 4, 3};
    };
}
