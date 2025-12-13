#pragma once

namespace i2c {
    enum class Endpoint {
        DeviceInformation = 0x01,
        DeviceName = 0x02,
        Registers = 0x03,
    };

    const uint8_t MAX_PACKET_SIZE = 32;
};
