#pragma once

#include <vector>
#include <memory>

#include <tfw/hal/time.h>

#include "IndicatorLed.h"
#include "SwitchIndicatorLed.h"
#include "IndicatorLedDriver.h"

namespace mkp::components::leds {
    class IndicatorLedManager {
    public:
        explicit IndicatorLedManager(uint16_t numberOfPixels, std::unique_ptr<IndicatorLedDriver> driver, tfw::hal::time::Clock& clock);
        ~IndicatorLedManager();

        void begin();
        void update();

        std::shared_ptr<IndicatorLed>& get(uint8_t pixelNumber);

        std::shared_ptr<SwitchIndicatorLed> connectToSwitch(uint8_t pixelNumber, const tfw::hal::buttons::Button& button);

        static std::unique_ptr<IndicatorLedManager> NeoPixel(
            uint16_t numberOfPixels,
            int16_t pin,
            tfw::hal::time::Clock& clock,
            neoPixelType type = NEO_GRB | NEO_KHZ800
        );

        static std::unique_ptr<IndicatorLedManager> NoOp(uint16_t numberOfPixels, tfw::hal::time::Clock& clock);

    private:
        std::unique_ptr<IndicatorLedDriver> driver;
        tfw::hal::time::Clock& clock;
        std::vector<std::shared_ptr<IndicatorLed>> leds;
        std::vector<std::shared_ptr<SwitchIndicatorLed>> switchIndicators;
    };
}