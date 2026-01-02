#pragma once

#include "firmwares/common/indicatorleds/IndicatorLed.h"
#include "firmwares/common/monitors/SwitchMonitor.h"

class SwitchIndicatorLed final : utils::observables::Observer<SwitchEvent> {
public:
    SwitchIndicatorLed(const SwitchMonitor& switchMonitor, IndicatorLed& indicatorLed);
    ~SwitchIndicatorLed() override;

private:
    void observe(const SwitchEvent& event) override;

    const SwitchMonitor& switchMonitor;
    IndicatorLed& indicatorLed;
};
