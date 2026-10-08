#pragma once
#include <pins_arduino.h>

#include "hal/base/Component.h"
#include "hal/leds/DeviceIdentificationFlash.h"
#include "hal/leds/IndicatorLeds.h"

namespace smc::firmwares {
    class Firmware : public hal::Component {
    public:
        explicit Firmware(const uint8_t numLeds)
            : leds(numLeds, PIN_PC5),
              identificationFlash(leds) {
        }

        ~Firmware() override = default;

        void setup() final {
            leds.setup();
            identificationFlash.setup();

            doSetup();
        }

        void update(const uint32_t deltaUs, events::EventQueue& queue) final {
            doUpdate(deltaUs, queue);
            // After doUpdate(), so the flash overrides the colors set by the firmware
            identificationFlash.update(deltaUs, queue);

            leds.update(deltaUs, queue);
        }

        virtual uint8_t deviceType() = 0;

        void flashIdentificationLights() {
            identificationFlash.start();
        }

    protected:
        virtual void doSetup() {}
        virtual void doUpdate(uint32_t deltaUs, events::EventQueue& queue) {}

        hal::leds::IndicatorLeds leds;

    private:
        hal::leds::DeviceIdentificationFlash identificationFlash;
    };
}