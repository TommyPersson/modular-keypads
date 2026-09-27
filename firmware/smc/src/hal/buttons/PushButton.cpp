#include "PushButton.h"

#include <Arduino.h>

using namespace smc::hal::buttons;

PushButton::PushButton(const uint8_t number, const uint8_t pin)
    : number(number),
      pin(pin) {
}

PushButton::~PushButton() = default;

void PushButton::setup() {
    pinMode(pin, INPUT_PULLUP);
}


void PushButton::update(uint32_t deltaUs, events::EventQueue& queue) {
    timeSinceUpdateUs += deltaUs;
    timeUntilLongPress = max(0, timeUntilLongPress - (int32_t)deltaUs);
    if (timeSinceUpdateUs < 10000) {
        return;
    }

    auto newState = readState();
    if (newState != state && state != State::UNKNOWN) {
        if (newState == State::PRESSED) {
            timeUntilLongPress = 500000;
            hasEmittedLongPress = false;
            queue.enqueue({.type = events::types::PUSH_BUTTON_PRESSED, .data = number});
        } else if (newState == State::UNPRESSED) {
            queue.enqueue({.type = events::types::PUSH_BUTTON_RELEASED, .data = number});
        }
    }

    state = newState;

    if (state == State::PRESSED && timeUntilLongPress == 0 && !hasEmittedLongPress) {
        hasEmittedLongPress = true;
        queue.enqueue({.type = events::types::PUSH_BUTTON_LONG_PRESSED, .data = number});
    }

    timeSinceUpdateUs = 0;
}

bool PushButton::isPressed() const {
    return state == State::PRESSED;
}

State PushButton::readState() const {
    const auto state = digitalRead(pin);
    if (state == LOW) {
        return State::PRESSED;
    }

    return State::UNPRESSED;
}
