#pragma once

#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/buttons/PushButton.h"

namespace smc::firmwares {
    class Smc2Firmware : public Firmware {
    public:
        Smc2Firmware();
        ~Smc2Firmware() override;

    protected:
        void doSetup() override;
        void doUpdate(uint32_t deltaUs, events::EventQueue& queue) override;

    private:
        hal::buttons::PushButton buttons[6] = {
            hal::buttons::PushButton(1, PIN_PA3),
            hal::buttons::PushButton(2, PIN_PA4),
            hal::buttons::PushButton(3, PIN_PA5),
            hal::buttons::PushButton(4, PIN_PB0),
            hal::buttons::PushButton(5, PIN_PB1),
            hal::buttons::PushButton(6, PIN_PB2),
        };

        uint8_t buttonLedMap[6] = {0, 1, 2, 5, 4, 3};
    };
}
