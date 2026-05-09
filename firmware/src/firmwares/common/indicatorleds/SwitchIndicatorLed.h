#pragma once

#include <tfw/hal/buttons/Button.h>

#include "firmwares/common/indicatorleds/IndicatorLed.h"

class SwitchIndicatorLed final : tfw::utils::observables::Observer<tfw::hal::buttons::ButtonStateChangedEvent> {
public:
    SwitchIndicatorLed(const tfw::hal::buttons::Button& button, IndicatorLed& indicatorLed);
    ~SwitchIndicatorLed() override;

private:
    void observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) override;

    const tfw::hal::buttons::Button& button;
    IndicatorLed& indicatorLed;
};
