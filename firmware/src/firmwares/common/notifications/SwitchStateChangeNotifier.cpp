#include "SwitchStateChangeNotifier.h"


SwitchStateChangeNotifier::SwitchStateChangeNotifier(Notifier& notifier) : notifier(notifier) {
}

SwitchStateChangeNotifier::~SwitchStateChangeNotifier() = default;

void SwitchStateChangeNotifier::observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) {
    if (event.toState == tfw::hal::buttons::ButtonState::PRESSED) {
        notifier.notify(
            {
                .type = "switch.pressed",
                .args = {std::to_string(event.buttonNumber)}
            }
            );
    } else if (event.toState == tfw::hal::buttons::ButtonState::UNPRESSED) {
        notifier.notify(
            {
                .type = "switch.released",
                .args = {std::to_string(event.buttonNumber)}
            }
            );
    }
}
