#pragma once

#include <Registers/Registers.h>

namespace devices::m::registers {
    inline RegisterDescriptor IOA{.name = "IOA", .index = 0};
    inline RegisterDescriptor IOB{.name = "IOB", .index = 1};
}
