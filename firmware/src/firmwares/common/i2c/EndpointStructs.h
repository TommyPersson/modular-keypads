#pragma once


namespace i2c::structs {
    #pragma pack(push, 1)
    struct DeviceInformation {
        char deviceId[16]{};
        char deviceType = 'g';
    };
    #pragma pack(pop)
}
