#include "RotaryEncoder.h"

#include <Arduino.h>

using namespace smc::hal::encoders;

namespace {
    constexpr uint8_t CW = smc::events::types::ROTARY_ENCODER_ROTATED_CLOCKWISE;
    constexpr uint8_t CCW = smc::events::types::ROTARY_ENCODER_ROTATED_COUNTER_CLOCKWISE;
}

RotaryEncoder::RotaryEncoder(const uint8_t number, const uint8_t pinA, const uint8_t pinB)
    : number(number),
      pinA(pinA),
      pinB(pinB) {
}

void RotaryEncoder::setup() {
    // The board has an RC circuit on the encoder pins, so no internal pull-ups
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);

    previousState = readState();
}

void RotaryEncoder::update(uint32_t deltaUs, events::EventQueue& queue) {
    rotation = events::types::NONE;

    const auto newState = readState();

    // Polled every loop rather than debounced, as missing a step loses a rotation
    if (newState == previousState) {
        if ((newState == 0b11 || newState == 0b00) && pendingEventType != events::types::NONE) {
            queue.enqueue({.type = pendingEventType, .data = number});
            rotation = pendingEventType;
            pendingEventType = events::types::NONE;
        }

        return;
    }

    switch (newState) {
    case 0b00:
        if (previousState == 0b01) {
            pendingEventType = CCW;
        } else if (previousState == 0b10) {
            pendingEventType = CW;
        }
        break;
    case 0b01:
        if (previousState == 0b11) {
            pendingEventType = CCW;
        } else if (previousState == 0b00) {
            pendingEventType = CW;
        }
        break;
    case 0b10:
        if (previousState == 0b00) {
            pendingEventType = CCW;
        } else if (previousState == 0b11) {
            pendingEventType = CW;
        }
        break;
    case 0b11:
        if (previousState == 0b10) {
            pendingEventType = CCW;
        } else if (previousState == 0b01) {
            pendingEventType = CW;
        }
        break;
    default:
        break;
    }

    previousState = newState;
}

uint8_t RotaryEncoder::readState() const {
    return digitalRead(pinA) << 1 | digitalRead(pinB);
}
