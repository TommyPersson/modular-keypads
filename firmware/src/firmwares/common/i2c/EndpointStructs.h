#pragma once


namespace i2c::structs {
#pragma pack(push, 1)
    struct DeviceInformation {
        char deviceId[16]{};
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


