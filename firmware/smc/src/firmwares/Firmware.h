#pragma once
#include <pins_arduino.h>

#include "hal/base/Component.h"
#include "hal/leds/IndicatorLeds.h"

namespace smc::firmwares {
    class Firmware : public hal::Component {
    public:
        explicit Firmware(const uint8_t numLeds)
            : leds(numLeds, PIN_PC5) {
        }

        ~Firmware() override = default;

        void setup() final {
            leds.setup();

            doSetup();
        }

        void update(const uint32_t deltaUs, events::EventQueue& queue) final {
            doUpdate(deltaUs, queue);

            leds.update(deltaUs, queue);
        }

    protected:
        virtual void doSetup() {}
        virtual void doUpdate(uint32_t deltaUs, events::EventQueue& queue) {}

        hal::leds::IndicatorLeds leds;
    };
}