#include "DeviceScanner.h"

#include "i2c/Endpoint.h"
#include "i2c/EndpointStructs.h"

DeviceScanner::DeviceScanner(I2cClient& client) :
    client(client) {
}

DeviceScanner::~DeviceScanner() {
}

std::vector<std::shared_ptr<DeviceProxy>> DeviceScanner::scan() {
    std::vector<std::shared_ptr<DeviceProxy>> result;

    for (uint8_t address = 10; address < 14; address++) {
        if (!client.setEndpoint(address, i2c::Endpoint::DeviceInformation)) {
            continue;
        }

        const auto deviceInformation = client.readEndpoint<i2c::structs::DeviceInformation>(address);

        const std::string id(deviceInformation->deviceId, 16);
        const char type = deviceInformation->deviceType;

        if (!client.setEndpoint(address, i2c::Endpoint::DeviceName)) {
            continue;
        }

        const auto deviceName = client.readEndpoint<i2c::structs::DeviceName>(address);
        const auto name = std::string(deviceName->deviceName, 32);

        DeviceConfiguration configuration{
            .id = id,
            .name = name,
            .type = type,
            .address = address
        };

        result.emplace_back(std::make_shared<DeviceProxy>(configuration, client));
    }

    return result;
}
