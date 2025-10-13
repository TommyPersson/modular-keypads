#pragma once

#include "firmwares/common/monitors/SwitchMonitor.h"

class IndicatorLed;

class SwitchIndicatorLed {
public:
    SwitchIndicatorLed(const SwitchMonitor& switchMonitor, IndicatorLed& indicatorLed);
    ~SwitchIndicatorLed();

    void update() const;

private:
    const SwitchMonitor& switchMonitor;
    IndicatorLed& indicatorLed;
};
