#include "SwitchIndicatorLed.h"

#include "IndicatorLed.h"

SwitchIndicatorLed::SwitchIndicatorLed(const SwitchMonitor& switchMonitor, IndicatorLed& indicatorLed) :
    switchMonitor(switchMonitor), indicatorLed(indicatorLed) {

    switchMonitor.onSwitchStateChanged().addObserver(this);
}

SwitchIndicatorLed::~SwitchIndicatorLed() {
    switchMonitor.onSwitchStateChanged().removeObserver(this);
}

void SwitchIndicatorLed::observe(const SwitchEvent& event) {
    // TODO "override behavior"?
    if (event.state == SwitchState::PRESSED) {
        indicatorLed.setColor(255, 255, 255, 0);
    } else {
        indicatorLed.setColor(0, 0, 0, 0);
    }
}
