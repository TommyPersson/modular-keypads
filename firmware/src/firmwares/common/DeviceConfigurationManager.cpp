#include "DeviceConfigurationManager.h"

#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceConfigurationManager");

    auto prefsNamespace = "device";
    auto prefsKeyDeviceId = "device.id";
    auto prefsKeyDeviceAddress = "device.address";
    auto prefsKeyDeviceName = "device.name";

    uint64_t generateNewId() {
        uint64_t newId;
        esp_fill_random(&newId, sizeof(newId));

        return newId;
    }
}

DeviceConfigurationManager::DeviceConfigurationManager(
    Preferences& preferences,
    devices::DeviceTypeDetector& deviceTypeDetector
) : preferences(preferences),
    deviceTypeDetector(deviceTypeDetector) {
}

void DeviceConfigurationManager::begin() const {
    this->preferences.begin(prefsNamespace, false);
    this->deviceTypeDetector.setup();

    const uint64_t existingId = preferences.getULong64(prefsKeyDeviceId);
    if (existingId == 0) {
        const auto newId = generateNewId();
        const auto numWritten = this->preferences.putULong64(prefsKeyDeviceId, newId);
        logger->info(
            "generated new device ID = '%08llx' (numWritten = %i)",
            newId,
            numWritten
        );
    } else {
        logger->info("read existing device ID = '%08llx'", existingId);
    }

    const uint8_t existingAddress = preferences.getUChar(prefsKeyDeviceAddress);
    if (existingAddress == 0) {
        const uint8_t newAddress = 1;
        const auto numWritten = this->preferences.putUChar(prefsKeyDeviceAddress, newAddress);
        logger->info(
            "generated new device address = '%0x02x' (numWritten = %i)",
            newAddress,
            numWritten
        );
    } else {
        logger->info("read existing device address = '0x%02x'", existingAddress);
    }

    this->preferences.end();
}

uint64_t DeviceConfigurationManager::getDeviceId() const {
    this->preferences.begin(prefsNamespace, true);
    auto id = this->preferences.getULong64(prefsKeyDeviceId);
    this->preferences.end();

    return id;
}

std::string DeviceConfigurationManager::getDeviceVersion() const {
    return "0.0.1";
}

uint8_t DeviceConfigurationManager::getDeviceAddress() const {
    this->preferences.begin(prefsNamespace, true);
    const auto address = this->preferences.getUChar(prefsKeyDeviceAddress);
    this->preferences.end();
    return address;
}

bool DeviceConfigurationManager::setDeviceAddress(const uint8_t deviceAddress) const {
    this->preferences.begin(prefsNamespace, false);
    const auto numWritten = this->preferences.putUChar(prefsKeyDeviceAddress, deviceAddress);
    this->preferences.end();
    return numWritten > 0;
}

char DeviceConfigurationManager::getDeviceType() const {
    return this->deviceTypeDetector.detectDeviceType();
}

std::string DeviceConfigurationManager::getDeviceName() const {
    this->preferences.begin(prefsNamespace, true);
    const auto deviceName = this->preferences.getString(prefsKeyDeviceName);
    this->preferences.end();

    if (deviceName.isEmpty()) {
        return "Unnamed";
    }

    return deviceName.c_str();
}

bool DeviceConfigurationManager::setDeviceName(const std::string_view& deviceName) const {
    this->preferences.begin(prefsNamespace, false);
    const auto numWritten = this->preferences.putString(prefsKeyDeviceName, std::string(deviceName).c_str());
    this->preferences.end();

    return numWritten > 0;
}

DeviceConfiguration DeviceConfigurationManager::getDeviceConfiguration() const {
    return {
        .id = getDeviceId(),
        .name = getDeviceName(),
        .type = getDeviceType(),
        .address = getDeviceAddress(),
    };
}
