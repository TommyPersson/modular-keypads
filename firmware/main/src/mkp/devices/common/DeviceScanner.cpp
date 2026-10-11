#include "DeviceScanner.h"

#include "mkp/devices/common/LocalDevice.h"

#include <tfw/hal/i2c.h>
#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceScanner");

    // Colliding devices usually separate in one or two rounds, as there are many temporary addresses
    constexpr int MAX_ADDRESS_ASSIGNMENT_ROUNDS = 5;

    // How long devices take to listen at a new address, as they change it from their loop
    constexpr uint32_t ADDRESS_CHANGE_DELAY_US = 5'000;
}

using namespace mkp::devices::common;

DeviceScanner::DeviceScanner(tfw::hal::i2c::Client& client, tfw::hal::time::Clock& clock) :
    client(client),
    clock(clock) {
}

DeviceScanner::~DeviceScanner() = default;

std::vector<std::shared_ptr<DeviceConfiguration>> DeviceScanner::scan() {
    logger->debug("Scanning...");

    assignAddresses();

    auto result = scanPermanentAddresses();

    // Learns the addresses of devices that were assigned them before the master restarted
    for (const auto& deviceConfiguration : result) {
        addressAllocator.remember(deviceConfiguration->id, deviceConfiguration->address);
    }

    logger->debug("Scanning complete!");

    return result;
}

void DeviceScanner::assignAddresses() {
    for (int round = 0; round < MAX_ADDRESS_ASSIGNMENT_ROUNDS; round++) {
        const auto result = assignAddressesOnce();

        if (result.numAssigned > 0 || result.numCollisions > 0) {
            clock.delayUs(ADDRESS_CHANGE_DELAY_US);
        }

        if (result.numCollisions == 0) {
            return;
        }
    }

    logger->warn("Devices still collide after %i rounds, leaving them for the next scan", MAX_ADDRESS_ASSIGNMENT_ROUNDS);
}

std::vector<uint8_t> DeviceScanner::findRespondingAddresses() {
    std::vector<uint8_t> result;

    for (uint8_t address = tfw::hal::i2c::addresses::PERMANENT_FIRST;
         address <= tfw::hal::i2c::addresses::TEMPORARY_LAST;
         address++) {
        if (client.probe(address)) {
            result.push_back(address);
        }
    }

    return result;
}

DeviceScanner::AssignmentRound DeviceScanner::assignAddressesOnce() {
    namespace addresses = tfw::hal::i2c::addresses;
    namespace builtin = tfw::hal::i2c::commands::builtin;

    AssignmentRound result;

    for (int address = addresses::TEMPORARY_FIRST; address <= addresses::TEMPORARY_LAST; address++) {
        if (!client.probe(address)) {
            continue;
        }

        const auto identityResult = client.readEndpoint(address, tfw::hal::i2c::endpoints::builtin::Identity);
        if (identityResult.has_error) {
            logger->info("Unable to read identity at 0x%02x: %s", address, identityResult.error_code);
            continue;
        }

        const auto identity = *identityResult.value;
        if (!identity.isValid()) {
            logger->info("Devices collided at temporary address 0x%02x, rerolling", address);
            client.sendCommand(address, 0, builtin::RerollAddress, {});
            result.numCollisions++;
            continue;
        }

        const auto permanentAddress = addressAllocator.allocate(identity.id, [this](const uint8_t candidate) {
            return !client.probe(candidate);
        });
        if (!permanentAddress.has_value()) {
            logger->error("No free address for device %08llx", identity.id);
            continue;
        }

        const auto assignResult = client.sendCommand(
            address,
            identity.id,
            builtin::AssignAddress,
            {.address = *permanentAddress}
        );
        if (assignResult.has_error) {
            logger->error("Unable to assign address 0x%02x to device %08llx", *permanentAddress, identity.id);
            continue;
        }

        logger->info("Assigned address 0x%02x to device %08llx", *permanentAddress, identity.id);
        result.numAssigned++;
    }

    return result;
}

std::vector<std::shared_ptr<DeviceConfiguration>> DeviceScanner::scanPermanentAddresses() {
    std::vector<std::shared_ptr<DeviceConfiguration>> result;

    for (uint8_t address = tfw::hal::i2c::addresses::PERMANENT_FIRST;
         address <= tfw::hal::i2c::addresses::PERMANENT_LAST;
         address++) {
        //logger->debug("probing %i", address);

        if (!client.probe(address)) {
            continue;
        }

        const auto deviceInformationResult = client.readEndpoint(address, devices::common::i2c::endpoints::DeviceInformation);
        if (deviceInformationResult.has_error) {
            logger->info("Unable to read device information at 0x%02x: %s", address, deviceInformationResult.error_code);
            continue;
        }

        const auto deviceInformation = deviceInformationResult.value;
        const auto id = deviceInformation->deviceId;
        const std::string type(1, deviceInformation->deviceType);

        const auto deviceNameResult = client.readEndpoint(address, devices::common::i2c::endpoints::DeviceName);
        if (deviceNameResult.has_error) {
            logger->info("Unable to read device name at 0x%02x: %s", address, deviceNameResult.error_code);
            continue;
        }


        const auto& deviceName = deviceNameResult.value->deviceName;
        const auto name = std::string(deviceName, strnlen(deviceName, sizeof(deviceName)));

        DeviceConfiguration configuration{
            .id = id,
            .name = name,
            .type = type,
            .address = address,
            .parentDeviceId = 0
        };

        result.emplace_back(std::make_shared<DeviceConfiguration>(configuration));
    }

    return result;
}
