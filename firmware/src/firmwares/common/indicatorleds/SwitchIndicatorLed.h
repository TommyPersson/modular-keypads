#pragma once

#include "firmwares/common/monitors/SwitchMonitor.h"

class IndicatorLed;

class SwitchIndicatorLed : Observer<SwitchEvent> {
public:
    SwitchIndicatorLed(const SwitchMonitor& switchMonitor, IndicatorLed& indicatorLed);
    ~SwitchIndicatorLed() override;

private:
    void observe(const SwitchEvent& event) override;

    const SwitchMonitor& switchMonitor;
    IndicatorLed& indicatorLed;
};
