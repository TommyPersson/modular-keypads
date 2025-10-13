#pragma once

#include <vector>
#include <memory>

#include "IndicatorLed.h"
#include "SwitchIndicatorLed.h"

class IndicatorLedManager {
public:
    explicit IndicatorLedManager(uint16_t numberOfPixels, int16_t pin, neoPixelType type = NEO_GRB | NEO_KHZ800);
    ~IndicatorLedManager();

    void begin();
    void update();
    void markAsDirty();
    void clearDirty();

    std::shared_ptr<IndicatorLed>& get(uint8_t pixelNumber);

    std::shared_ptr<SwitchIndicatorLed> connectToSwitch(uint8_t pixelNumber, const SwitchMonitor& switchMonitor);

    static std::unique_ptr<IndicatorLedManager> NeoPixel(
        uint16_t numberOfPixels,
        int16_t pin,
        neoPixelType type = NEO_GRB | NEO_KHZ800
    );

private:
    Adafruit_NeoPixel neoPixel;
    std::vector<std::shared_ptr<IndicatorLed>> leds;
    std::vector<std::shared_ptr<SwitchIndicatorLed>> switchIndicators;
    bool isDirty = true;
};
