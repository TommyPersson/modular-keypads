#pragma once

#include <string>

namespace utils::registers {
    struct RegisterDescriptor {
        std::string name;
        uint8_t index;
    };
}