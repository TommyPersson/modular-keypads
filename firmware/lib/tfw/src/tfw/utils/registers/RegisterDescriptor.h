#pragma once

#include <string>

namespace tfw::utils::registers {
    struct RegisterDescriptor {
        std::string name;
        uint8_t index;
    };
}