#pragma once

#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/buttons/PushButtonWithIndicator.h"

namespace smc::firmwares {
    class Smc1Firmware : public Firmware {
    public:
        Smc1Firmware();
        ~Smc1Firmware() override;

    protected:
        void doSetup() override;
        void doUpdate(uint32_t deltaUs, events::EventQueue& queue) override;

    private:
        hal::buttons::PushButtonWithIndicator buttons[4] = {
            hal::buttons::PushButtonWithIndicator(1, PIN_PA3, leds, 0),
            hal::buttons::PushButtonWithIndicator(2, PIN_PA4, leds, 1),
            hal::buttons::PushButtonWithIndicator(3, PIN_PB0, leds, 3),
            hal::buttons::PushButtonWithIndicator(4, PIN_PB1, leds, 2),
        };
    };
}
