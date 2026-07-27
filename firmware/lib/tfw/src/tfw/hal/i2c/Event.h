#pragma once

#include <cstdint>

namespace tfw::hal::i2c {

#pragma pack(push, 1)
    struct Event {
        uint8_t type;
        uint64_t deviceId;
        uint8_t args[8];
    };
#pragma pack(pop)

}
