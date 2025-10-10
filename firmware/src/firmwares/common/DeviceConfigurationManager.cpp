#include "DeviceConfigurationManager.h"

auto prefsNamespace = "device";
auto prefsKeyDeviceId = "device.id";
auto prefsKeyDeviceAddress = "device.address";
auto prefsKeyDeviceType = "device.type";

String generateNewId() {
    uint8_t newIdBytes[16];
    esp_fill_random(newIdBytes, sizeof(newIdBytes));

    char newId[sizeof(newIdBytes) * 2 + 1];
    for (int i = 0; i < sizeof(newIdBytes); i++) {
        sprintf(&newId[i * 2], "%02x", newIdBytes[i]);
    }
    newId[sizeof(newId) - 1] = '\0';

    return newId;
}


DeviceConfigurationManager::DeviceConfigurationManager(Preferences& preferences, Logger& logger) :
    preferences(preferences),
    logger(logger) {
}

void DeviceConfigurationManager::begin() const {
    this->preferences.begin(prefsNamespace, false);

    const String existingId = preferences.getString(prefsKeyDeviceId);
    if (existingId.isEmpty()) {
        const auto newId = generateNewId();
        const auto numWritten = this->preferences.putString(prefsKeyDeviceId, newId);
        logger.info(
            "DeviceConfigurationManager:generated new device ID = '%s' (numWritten = %i)",
            newId.c_str(),
            numWritten
            );
    } else {
        logger.info("DeviceConfigurationManager:read existing device ID = '%s'", existingId.c_str());
    }

    const uint8_t existingAddress = preferences.getUChar(prefsKeyDeviceAddress);
    if (existingAddress == 0) {
        const uint8_t newAddress = 1;
        const auto numWritten = this->preferences.putUChar(prefsKeyDeviceAddress, newAddress);
        logger.info(
            "DeviceConfigurationManager:generated new device address = '%0x02x' (numWritten = %i)",
            newAddress,
            numWritten
            );
    } else {
        logger.info("DeviceConfigurationManager:read existing device address = '0x%02x'", existingAddress);
    }

    this->preferences.end();
}

std::string DeviceConfigurationManager::getDeviceId() const {
    this->preferences.begin(prefsNamespace, true);
    auto id = this->preferences.getString(prefsKeyDeviceId);
    this->preferences.end();

    return id.c_str();
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

    logger.debug("numWritten = %i", numWritten);

    this->preferences.begin(prefsNamespace, false);
    const auto written = this->preferences.getUChar(prefsKeyDeviceType, deviceType);
    this->preferences.end();

    return written == deviceType;
}
