#include "DeviceName.h"

#include <EEPROM.h>
#include <string.h>

using namespace smc::hal::identity;

namespace {
    constexpr uint8_t EEPROM_ADDRESS = 0;

    bool isPrintable(const uint8_t c) {
        return c >= 0x20 && c <= 0x7E;
    }
}

void smc::hal::identity::normalizeDeviceName(uint8_t (&name)[DEVICE_NAME_LENGTH]) {
    name[DEVICE_NAME_LENGTH - 1] = 0;

    const auto length = strnlen(reinterpret_cast<const char*>(name), DEVICE_NAME_LENGTH);
    bool isValid = length > 0;
    for (uint8_t i = 0; i < length; i++) {
        isValid = isValid && isPrintable(name[i]);
    }

    if (!isValid) {
        memset(name, 0, sizeof(name));
        memcpy(name, DEFAULT_DEVICE_NAME, sizeof(DEFAULT_DEVICE_NAME));
        return;
    }

    memset(name + length, 0, DEVICE_NAME_LENGTH - length);
}

void DeviceNameStorage::load(uint8_t (&name)[DEVICE_NAME_LENGTH]) const {
    for (uint8_t i = 0; i < DEVICE_NAME_LENGTH; i++) {
        name[i] = EEPROM.read(EEPROM_ADDRESS + i);
    }

    normalizeDeviceName(name);
}

void DeviceNameStorage::store(const uint8_t (&name)[DEVICE_NAME_LENGTH]) {
    memcpy(pendingName, name, sizeof(pendingName));
    nextIndex = 0;
    isStoring = true;
}

bool DeviceNameStorage::update() {
    if (!isStoring || NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm) {
        return false;
    }

    while (nextIndex < DEVICE_NAME_LENGTH) {
        const uint8_t i = nextIndex++;
        if (EEPROM.read(EEPROM_ADDRESS + i) != pendingName[i]) {
            EEPROM.write(EEPROM_ADDRESS + i, pendingName[i]);
            return false;
        }
    }

    isStoring = false;
    return true;
}
