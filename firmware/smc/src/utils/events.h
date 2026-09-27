#pragma once

#include <stdint.h>

#include "ring_queue.h"

namespace smc::events {
    namespace types {
        inline constexpr uint8_t NONE = 0;
        inline constexpr uint8_t PUSH_BUTTON_PRESSED = 1;
        inline constexpr uint8_t PUSH_BUTTON_RELEASED = 2;
        inline constexpr uint8_t PUSH_BUTTON_LONG_PRESSED = 3;
    }

    struct Event {
        uint8_t type;
        uint8_t data;
    };

    using EventQueue = utils::ring_queue<Event, 32>;
}
