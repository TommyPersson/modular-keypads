#pragma once

namespace utils::i2c {
    enum class Endpoint {
        DeviceInformation = 0x01,
        DeviceName = 0x02,
        Registers = 0x03,
    };

    const uint8_t MAX_PACKET_SIZE = 30; // 32 is the underlying Wire buffer, but if we use it we often read ETB characters
};
