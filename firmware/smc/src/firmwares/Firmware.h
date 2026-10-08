#pragma once
#include <pins_arduino.h>

#include "hal/base/Component.h"
#include "hal/leds/ButtonIdentificationFlash.h"
#include "hal/leds/DeviceIdentificationFlash.h"
#include "hal/leds/IndicatorLeds.h"

namespace smc::firmwares {
    class Firmware : public hal::Component {
    public:
        explicit Firmware(const uint8_t numLeds)
            : leds(numLeds, PIN_PC5),
              deviceIdentificationFlash(leds),
              buttonIdentificationFlash(leds) {
        }

        ~Firmware() override = default;

        void setup() final {
            leds.setup();
            deviceIdentificationFlash.setup();
            buttonIdentificationFlash.setup();

            doSetup();
        }

        void update(const uint32_t deltaUs, events::EventQueue& queue) final {
            doUpdate(deltaUs, queue);
            // After doUpdate(), so the flashes override the colors set by the firmware
            deviceIdentificationFlash.update(deltaUs, queue);
            buttonIdentificationFlash.update(deltaUs, queue);

            leds.update(deltaUs, queue);
        }

        virtual uint8_t deviceType() = 0;

        void flashIdentificationLights() {
            deviceIdentificationFlash.start();
        }

        void flashButtonIdentificationLight(const uint8_t buttonNumber) {
            const auto ledIndex = ledIndexForButton(buttonNumber);
            if (ledIndex >= 0) {
                buttonIdentificationFlash.start(ledIndex);
            }
        }

    protected:
        virtual void doSetup() {}
        virtual void doUpdate(uint32_t deltaUs, events::EventQueue& queue) {}

        // Returns -1 when there is no such button, or it has no LED
        virtual int8_t ledIndexForButton(uint8_t buttonNumber) { return -1; }

        hal::leds::IndicatorLeds leds;

    private:
        hal::leds::DeviceIdentificationFlash deviceIdentificationFlash;
        hal::leds::ButtonIdentificationFlash buttonIdentificationFlash;
    };
}