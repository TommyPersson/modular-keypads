#pragma once

#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/buttons/PushButton.h"

namespace smc::firmwares {
    class Smc1Firmware : public Firmware {
    public:
        Smc1Firmware();
        ~Smc1Firmware() override;

    protected:
        void doSetup() override;
        void doUpdate(uint32_t deltaUs, events::EventQueue& queue) override;

    private:
        hal::buttons::PushButton buttons[4] = {
            hal::buttons::PushButton(1, PIN_PA3),
            hal::buttons::PushButton(2, PIN_PA4),
            hal::buttons::PushButton(3, PIN_PB0),
            hal::buttons::PushButton(4, PIN_PB1),
        };

        uint8_t buttonLedMap[4] = {0, 1, 3, 2};
    };
}
