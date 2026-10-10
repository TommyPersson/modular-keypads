#include "SwitchIndicatorLed.h"

using namespace tfw::hal::buttons;
using namespace mkp::components::leds;

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
    if (event.toState == ButtonState::PRESSED || event.toState == ButtonState::LONG_PRESSED) {
        indicatorLed.setColor(255, 255, 255, 0);
    } else {
        indicatorLed.setColor(0, 0, 0, 0);
    }
}
