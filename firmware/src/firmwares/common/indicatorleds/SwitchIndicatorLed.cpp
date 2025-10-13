#include "SwitchIndicatorLed.h"

#include "IndicatorLed.h"

SwitchIndicatorLed::SwitchIndicatorLed(const SwitchMonitor& switchMonitor, IndicatorLed& indicatorLed) :
    switchMonitor(switchMonitor), indicatorLed(indicatorLed) {
}

SwitchIndicatorLed::~SwitchIndicatorLed() = default;

void SwitchIndicatorLed::update() const {
    // TODO "override behavior"?
    // i.e. let a i2c writable register control the color normally, but presses should overwrite temporarily.
  if (switchMonitor.getCurrentState() == SwitchState::PRESSED) {
        indicatorLed.setColor(255, 255, 255, 0);
    } else {
        indicatorLed.setColor(0, 0, 0, 0);
    }
}
