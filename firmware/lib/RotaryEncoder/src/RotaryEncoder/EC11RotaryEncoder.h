#pragma once

#include <cstdint>

enum class EncoderDirection { CW, CCW, NONE };

class EC11RotaryEncoder {
public:
    explicit EC11RotaryEncoder();
    ~EC11RotaryEncoder();

    EncoderDirection update(uint8_t a, uint8_t b);
    EncoderDirection getDirection() const;

private:
    uint8_t lastState = 0x00;
    EncoderDirection lastDirection = EncoderDirection::NONE;
};