#include "DeviceConfigurationManager.h"

namespace {
    auto logger = common::logging::createLogger("DeviceConfigurationManager");

    auto prefsNamespace = "device";
    auto prefsKeyDeviceId = "device.id";
    auto prefsKeyDeviceAddress = "device.address";
    auto prefsKeyDeviceType = "device.type";
    auto prefsKeyDeviceName = "device.name";

    uint64_t generateNewId() {
        uint64_t newId;
        esp_fill_random(&newId, sizeof(newId));

        return newId;
    }
}

DeviceConfigurationManager::DeviceConfigurationManager(Preferences& preferences) :
    preferences(preferences) {
}

void DeviceConfigurationManager::begin() const {
    this->preferences.begin(prefsNamespace, false);

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
    this->preferences.begin(prefsNamespace, true);
    const auto deviceType = this->preferences.getUChar(prefsKeyDeviceType);
    this->preferences.end();

    return deviceType;
}

bool DeviceConfigurationManager::setDeviceType(const char deviceType) const {
    this->preferences.begin(prefsNamespace, false);
    const auto numWritten = this->preferences.putUChar(prefsKeyDeviceType, deviceType);
    this->preferences.end();

    logger->debug("numWritten = %i", numWritten);

    this->preferences.begin(prefsNamespace, false);
    const auto written = this->preferences.getUChar(prefsKeyDeviceType, deviceType);
    this->preferences.end();

    return written == deviceType;
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
