#ifdef ARDUINO

#include "PreferencesDeviceSettings.h"

#include <tfw/hal/logging.h>

using namespace tfw::hal::settings;

namespace {
    auto logger = tfw::hal::logging::createLogger("PreferencesDeviceSettings");

    // Keys are short, so they're copied to the stack to get null terminated strings. Invalid keys are rejected rather
    // than truncated, as truncated keys could silently collide with each other.
    struct Key {
        explicit Key(const std::string_view key) : isValid(!key.empty() && key.length() <= MAX_KEY_LENGTH) {
            if (!isValid) {
                logger->error(
                    "invalid key '%.*s', must be 1 to %u characters",
                    static_cast<int>(key.length()),
                    key.data(),
                    static_cast<unsigned int>(MAX_KEY_LENGTH)
                );
                buffer[0] = '\0';
                return;
            }

            key.copy(buffer, key.length());
            buffer[key.length()] = '\0';
        }

        const char* c_str() const { return buffer; }

        const bool isValid;
        char buffer[MAX_KEY_LENGTH + 1];
    };
}

PreferencesDeviceSettings::PreferencesDeviceSettings(const char* namespaceName) : namespaceName(namespaceName) {
}

PreferencesDeviceSettings::~PreferencesDeviceSettings() {
    if (isStarted) {
        preferences.end();
    }
}

bool PreferencesDeviceSettings::begin() {
    if (!isStarted) {
        isStarted = preferences.begin(namespaceName, false);
    }

    return isStarted;
}

bool PreferencesDeviceSettings::isKey(const std::string_view key) {
    const Key k(key);
    return k.isValid && preferences.isKey(k.c_str());
}

bool PreferencesDeviceSettings::remove(const std::string_view key) {
    const Key k(key);
    return k.isValid && preferences.remove(k.c_str());
}

uint32_t PreferencesDeviceSettings::getUInt(const std::string_view key, const uint32_t defaultValue) {
    const Key k(key);
    return k.isValid ? preferences.getUInt(k.c_str(), defaultValue) : defaultValue;
}

bool PreferencesDeviceSettings::putUInt(const std::string_view key, const uint32_t value) {
    const Key k(key);
    return k.isValid && preferences.putUInt(k.c_str(), value) > 0;
}

int32_t PreferencesDeviceSettings::getInt(const std::string_view key, const int32_t defaultValue) {
    const Key k(key);
    return k.isValid ? preferences.getInt(k.c_str(), defaultValue) : defaultValue;
}

bool PreferencesDeviceSettings::putInt(const std::string_view key, const int32_t value) {
    const Key k(key);
    return k.isValid && preferences.putInt(k.c_str(), value) > 0;
}

bool PreferencesDeviceSettings::getBool(const std::string_view key, const bool defaultValue) {
    const Key k(key);
    return k.isValid ? preferences.getBool(k.c_str(), defaultValue) : defaultValue;
}

bool PreferencesDeviceSettings::putBool(const std::string_view key, const bool value) {
    const Key k(key);
    return k.isValid && preferences.putBool(k.c_str(), value) > 0;
}

#endif // ARDUINO
