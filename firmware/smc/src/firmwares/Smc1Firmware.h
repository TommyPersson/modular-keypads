#pragma once

#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/buttons/PushButton.h"
#include "hal/leds/IndicatorLeds.h"

namespace smc::firmwares {
    class Smc1Firmware : public Firmware {
    public:
        Smc1Firmware();
        ~Smc1Firmware() override;
        void setup() override;
        void update(uint32_t deltaUs, smc::events::EventQueue& queue) override;

    private:
        smc::hal::buttons::PushButton buttons[4] = {
            smc::hal::buttons::PushButton(1, PIN_PA3),
            smc::hal::buttons::PushButton(2, PIN_PA4),
            smc::hal::buttons::PushButton(3, PIN_PB0),
            smc::hal::buttons::PushButton(4, PIN_PB1),
        };

        smc::hal::leds::IndicatorLeds leds = smc::hal::leds::IndicatorLeds(4, PIN_PC5);

        uint8_t buttonLedMap[4] = {0, 1, 3, 2};
    };
}
