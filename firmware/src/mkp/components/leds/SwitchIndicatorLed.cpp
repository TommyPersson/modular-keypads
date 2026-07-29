#include "SwitchIndicatorLed.h"

using namespace tfw::hal::buttons;

SwitchIndicatorLed::SwitchIndicatorLed(
    const Button& button,
    IndicatorLed& indicatorLed
) : button(button),
    indicatorLed(indicatorLed) {
    button.onStateChanged().addObserver(this);
}

SwitchIndicatorLed::~SwitchIndicatorLed() {
    button.onStateChanged().removeObserver(this);
}

void SwitchIndicatorLed::observe(const ButtonStateChangedEvent& event) {
    // TODO "override behavior"?
    if (event.toState == tfw::hal::buttons::ButtonState::PRESSED) {
        indicatorLed.setColor(255, 255, 255, 0);
    } else {
        indicatorLed.setColor(0, 0, 0, 0);
    }
}
