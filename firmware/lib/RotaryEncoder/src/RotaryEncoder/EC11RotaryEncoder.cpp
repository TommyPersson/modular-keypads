#include "EC11RotaryEncoder.h"

#include <HardwareSerial.h>

EC11RotaryEncoder::EC11RotaryEncoder() {
}

EC11RotaryEncoder::~EC11RotaryEncoder() {
}

EncoderDirection EC11RotaryEncoder::update(const uint8_t a, const uint8_t b) {
    const uint8_t state = (a << 1) | b;

    if (state == lastState) {
        if (state == 0b11 || state == 0b00) {
            if (lastDirection != EncoderDirection::NONE) {
                const EncoderDirection direction = lastDirection;
                lastDirection = EncoderDirection::NONE;
                return direction;
            }
        }

        return EncoderDirection::NONE;
    }

    switch (state) {
    case 0b00:
        if (lastState == 0b01) {
            lastDirection = EncoderDirection::CW;
        } else if (lastState == 0b10) {
            lastDirection = EncoderDirection::CCW;
        }
        break;
    case 0b01:
        if (lastState == 0b11) {
            lastDirection = EncoderDirection::CW;
        } else if (lastState == 0b00) {
            lastDirection = EncoderDirection::CCW;
        }
        break;
    case 0b10:
        if (lastState == 0b00) {
            lastDirection = EncoderDirection::CW;
        } else if (lastState == 0b11) {
            lastDirection = EncoderDirection::CCW;
        }
        break;
    case 0b11:
        if (lastState == 0b10) {
            lastDirection = EncoderDirection::CW;
        } else if (lastState == 0b01) {
            lastDirection = EncoderDirection::CCW;
        }
        break;
    default:
        break;
    }

    lastState = state;

    return EncoderDirection::NONE;
}

EncoderDirection EC11RotaryEncoder::getDirection() const {
    return lastDirection;
}

