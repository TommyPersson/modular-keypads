#pragma once
#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/leds/IndicatorLeds.h"

namespace smc::firmwares {
    class UnknownFirmware : public Firmware {
    public:
        explicit UnknownFirmware(uint8_t smcType);
        UnknownFirmware() = default;

        void setup() override;

    private:
        int smcType{};

        // No LEDs, but drives the data line low so the LEDs don't latch noise
        smc::hal::leds::IndicatorLeds leds = smc::hal::leds::IndicatorLeds(0, PIN_PC5);
    };
}
