#include "CachedDeviceSettings.h"

using namespace tfw::hal::settings;

CachedDeviceSettings::CachedDeviceSettings(DeviceSettings& backing) : backing(backing) {
}

bool CachedDeviceSettings::begin() {
    cache.clear();
    return backing.begin();
}

bool CachedDeviceSettings::isKey(const std::string_view key) {
    const auto it = cache.find(key);
    if (it != cache.end()) {
        return !std::holds_alternative<std::monostate>(it->second);
    }

    // The type of an existing value isn't known here, so only missing keys are cached
    const auto exists = backing.isKey(key);
    if (!exists) {
        cache.insert_or_assign(std::string(key), std::monostate{});
    }

    return exists;
}

bool CachedDeviceSettings::remove(const std::string_view key) {
    const auto removed = backing.remove(key);
    store(key, removed, std::monostate{});
    return removed;
}

uint32_t CachedDeviceSettings::getUInt(const std::string_view key, const uint32_t defaultValue) {
    return get<uint32_t>(key, defaultValue, [this](auto k, auto d) { return backing.getUInt(k, d); });
}

bool CachedDeviceSettings::putUInt(const std::string_view key, const uint32_t value) {
    const auto written = backing.putUInt(key, value);
    store(key, written, value);
    return written;
}

int32_t CachedDeviceSettings::getInt(const std::string_view key, const int32_t defaultValue) {
    return get<int32_t>(key, defaultValue, [this](auto k, auto d) { return backing.getInt(k, d); });
}

bool CachedDeviceSettings::putInt(const std::string_view key, const int32_t value) {
    const auto written = backing.putInt(key, value);
    store(key, written, value);
    return written;
}

bool CachedDeviceSettings::getBool(const std::string_view key, const bool defaultValue) {
    return get<bool>(key, defaultValue, [this](auto k, auto d) { return backing.getBool(k, d); });
}

bool CachedDeviceSettings::putBool(const std::string_view key, const bool value) {
    const auto written = backing.putBool(key, value);
    store(key, written, value);
    return written;
}

template <typename T, typename TReader>
T CachedDeviceSettings::get(const std::string_view key, const T defaultValue, TReader readBacking) {
    const auto it = cache.find(key);
    if (it != cache.end()) {
        if (std::holds_alternative<std::monostate>(it->second)) {
            return defaultValue;
        }

        // A value cached as another type is read again, as the underlying storage decides how that's handled
        if (const auto value = std::get_if<T>(&it->second)) {
            return *value;
        }
    }

    if (!backing.isKey(key)) {
        cache.insert_or_assign(std::string(key), std::monostate{});
        return defaultValue;
    }

    T value = readBacking(key, defaultValue);
    cache.insert_or_assign(std::string(key), value);
    return value;
}

void CachedDeviceSettings::store(const std::string_view key, const bool written, CachedValue value) {
    if (written) {
        cache.insert_or_assign(std::string(key), value);
    } else {
        // The state of the underlying storage is unknown after a failed write
        if (const auto it = cache.find(key); it != cache.end()) {
            cache.erase(it);
        }
    }
}
