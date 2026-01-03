#pragma once

#include <cstdint>

namespace utils::i2c {
    struct EndpointDescriptor {
        std::uint8_t id;
        std::uint8_t length;
    };

    const std::uint8_t MAX_PACKET_SIZE = 30; // 32 is the underlying Wire buffer, but if we use it we often read ETB characters
};
