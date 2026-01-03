#pragma once

#include <cstdint>

namespace utils::i2c {
    template <class TStruct>
    struct EndpointDescriptor {
        std::uint8_t id = 0;
        std::uint8_t length = sizeof(TStruct);
    };

    const std::uint8_t MAX_PACKET_SIZE = 30; // 32 is the underlying Wire buffer, but if we use it we often read ETB characters
};
