#include "SwitchStateChangeNotifier.h"


SwitchStateChangeNotifier::SwitchStateChangeNotifier(Notifier& notifier) : notifier(notifier) {
}

SwitchStateChangeNotifier::~SwitchStateChangeNotifier() = default;

void SwitchStateChangeNotifier::observe(const SwitchEvent& event) {
    if (event.state == SwitchState::PRESSED) {
        notifier.notify(
            {
                .type = "switch.pressed",
                .args = {std::to_string(event.switchNumber)}
            }
            );
    } else if (event.state == SwitchState::UNPRESSED) {
        notifier.notify(
            {
                .type = "switch.released",
                .args = {std::to_string(event.switchNumber)}
            }
            );
    }
}
