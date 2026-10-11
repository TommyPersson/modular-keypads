#include "DeviceConfigurationManager.h"

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceConfigurationManager");

    auto prefsNamespace = "device";
    auto prefsKeyDeviceId = "device.id";
    auto prefsKeyDeviceName = "device.name";

    uint64_t generateNewId() {
        uint64_t newId;
        esp_fill_random(&newId, sizeof(newId));

        return newId;   
    }
}

using namespace mkp::devices::common;

DeviceConfigurationManager::DeviceConfigurationManager(
    Preferences& preferences,
    DeviceTypeDetector& deviceTypeDetector
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

std::string DeviceConfigurationManager::getDeviceType() const {
    return std::string(1, this->deviceTypeDetector.detectDeviceType());
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
        // The local device either is the master, which has no address, or gets one assigned by the master
        .address = 0,
        .parentDeviceId = 0,
    };
}
