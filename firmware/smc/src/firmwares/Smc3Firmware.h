#pragma once

#include <pins_arduino.h>

#include "Firmware.h"
#include "hal/buttons/PushButton.h"
#include "hal/encoders/RotaryEncoder.h"

namespace smc::firmwares {
    class Smc3Firmware : public Firmware {
    public:
        Smc3Firmware();
        ~Smc3Firmware() override;

        uint8_t deviceType() override { return 0x03; }

    protected:
        void doSetup() override;
        void doUpdate(uint32_t deltaUs, events::EventQueue& queue) override;
        int8_t ledIndexForButton(uint8_t buttonNumber) override;

    private:
        static constexpr uint8_t CLOCKWISE_LED_INDEX = 1;
        static constexpr uint8_t COUNTER_CLOCKWISE_LED_INDEX = 0;
        static constexpr uint32_t ROTATION_LED_ON_US = 100000;
        static constexpr uint8_t LED_BRIGHTNESS = 50;
        static constexpr uint8_t LED_BRIGHTNESS_LONG_PRESSED = 200;

        void updateLed(uint8_t ledIndex, uint32_t& remainingUs, uint32_t deltaUs);

        hal::encoders::RotaryEncoder encoder = hal::encoders::RotaryEncoder(1, PIN_PA3, PIN_PA4);
        hal::buttons::PushButton button = hal::buttons::PushButton(1, PIN_PA5);

        uint32_t clockwiseLedRemainingUs = 0;
        uint32_t counterClockwiseLedRemainingUs = 0;
    };
}
