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
        char deviceName[32]{};
    };
#pragma pack(pop)
}
