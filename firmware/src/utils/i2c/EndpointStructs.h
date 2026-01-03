#pragma once
#include "Endpoint.h"


namespace utils::i2c::structs {
#pragma pack(push, 1)
    struct DeviceInformation {
        uint64_t deviceId;
        char deviceType = 'g';
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct DeviceName {
        char deviceName[MAX_PACKET_SIZE]{};
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct DeviceRegisters {
        uint8_t data[MAX_PACKET_SIZE]{};
    };
#pragma pack(pop)
}


