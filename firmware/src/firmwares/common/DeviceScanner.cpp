#include "DeviceScanner.h"

#include <firmwares/modules/common/DeviceModule.h>

#include "utils/i2c/Endpoint.h"
#include "utils/strings.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceScanner");
}

DeviceScanner::DeviceScanner(utils::i2c::Client& client) :
    client(client) {
}

DeviceScanner::~DeviceScanner() = default;

std::vector<std::shared_ptr<DeviceConfiguration>> DeviceScanner::scan() {
    std::vector<std::shared_ptr<DeviceConfiguration>> result;

    for (uint8_t address = 10; address < 14; address++) {
        logger->debug("probing %i", address);

        const auto deviceInformationResult = client.readEndpoint(address, devices::common::i2c::endpoints::DeviceInformation);
        if (deviceInformationResult.has_error) {
            logger->debug("unable to get device name (%i): %s", address, deviceInformationResult.error_code);
            continue;
        }

        const auto deviceInformation = deviceInformationResult.value;
        const auto id = deviceInformation->deviceId;
        const char type = deviceInformation->deviceType;

        const auto deviceNameResult = client.readEndpoint(address, devices::common::i2c::endpoints::DeviceName);
        if (deviceNameResult.has_error) {
            logger->debug("unable to get device name (%i): %s", address, deviceNameResult.error_code);
            continue;
        }


        const auto name = std::string(deviceNameResult.value->deviceName, sizeof(deviceNameResult.value->deviceName));

        DeviceConfiguration configuration{
            .id = id,
            .name = name,
            .type = type,
            .address = address
        };

        result.emplace_back(std::make_shared<DeviceConfiguration>(configuration));
    }

    return result;
}
