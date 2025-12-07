#pragma once


namespace i2c::structs {
    #pragma pack(1)
    struct DeviceInformation {
        char deviceId[16]{};
        char deviceType = 'g';
    };
}
