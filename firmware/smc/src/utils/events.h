#pragma once

#include <stdint.h>

#include "ring_queue.h"

namespace smc::events {
    namespace types {
        inline constexpr uint8_t NONE = 0;
        inline constexpr uint8_t BOOTED = 1;
        inline constexpr uint8_t DEVICE_NAME_STORED = 2;
        inline constexpr uint8_t PUSH_BUTTON_PRESSED = 11;
        inline constexpr uint8_t PUSH_BUTTON_RELEASED = 12;
        inline constexpr uint8_t PUSH_BUTTON_LONG_PRESSED = 13;
        inline constexpr uint8_t ROTARY_ENCODER_ROTATED_CLOCKWISE = 21;
        inline constexpr uint8_t ROTARY_ENCODER_ROTATED_COUNTER_CLOCKWISE = 22;
    }

    struct Event {
        uint8_t type = 0;
        uint8_t data = 0;
    };

    using EventQueue = utils::ring_queue<Event, 32>;
}
