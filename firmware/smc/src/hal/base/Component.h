#pragma once

#include <stdint.h>
#include "utils/events.h"

namespace smc::hal {
    class Component {
    public:
        virtual ~Component() = default;
        virtual void setup() = 0;
        virtual void update(uint32_t deltaUs, events::EventQueue& queue) = 0;
    };
}