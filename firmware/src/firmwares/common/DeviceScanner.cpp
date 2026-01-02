#include "DeviceScanner.h"

#include "i2c/Endpoint.h"
#include "i2c/EndpointStructs.h"
#include "utils/strings.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = common::logging::createLogger("DeviceScanner");
}

DeviceScanner::DeviceScanner(I2cClient& client) :
    client(client) {
}

DeviceScanner::~DeviceScanner() = default;

std::vector<std::shared_ptr<DeviceProxy>> DeviceScanner::scan() {
    std::vector<std::shared_ptr<DeviceProxy>> result;

    for (uint8_t address = 10; address < 14; address++) {
        logger->debug("probing %i", address);

        if (!client.setEndpoint(address, i2c::Endpoint::DeviceInformation)) {
            logger->debug("unable to get device information (%i)", address);
            continue;
        }

        const auto deviceInformation = client.readEndpoint<i2c::structs::DeviceInformation>(address);

        const auto id = deviceInformation->deviceId;
        const char type = deviceInformation->deviceType;

        if (!client.setEndpoint(address, i2c::Endpoint::DeviceName)) {
            logger->debug("unable to get device name (%i)", address);
            continue;
        }

        const auto deviceName = client.readEndpoint<i2c::structs::DeviceName>(address);
        const auto name = std::string(deviceName->deviceName, sizeof(deviceName->deviceName));

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
