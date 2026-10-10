#pragma once

#include <map>
#include <string>
#include <variant>

#include "DeviceSettings.h"

namespace tfw::hal::settings {
    /**
     * In-memory DeviceSettings for tests. Counts reads and writes, so that caching can be verified.
     */
    class MockDeviceSettings final : public DeviceSettings {
    public:
        bool begin() override { return true; }

        bool isKey(const std::string_view key) override {
            numReads++;
            return values.contains(key);
        }

        bool remove(const std::string_view key) override {
            numWrites++;
            const auto it = values.find(key);
            if (it == values.end()) {
                return false;
            }

            values.erase(it);
            return true;
        }

        uint32_t getUInt(const std::string_view key, const uint32_t defaultValue) override {
            return get(key, defaultValue);
        }

        bool putUInt(const std::string_view key, const uint32_t value) override { return put(key, value); }

        int32_t getInt(const std::string_view key, const int32_t defaultValue) override {
            return get(key, defaultValue);
        }

        bool putInt(const std::string_view key, const int32_t value) override { return put(key, value); }

        bool getBool(const std::string_view key, const bool defaultValue) override { return get(key, defaultValue); }

        bool putBool(const std::string_view key, const bool value) override { return put(key, value); }

        int numReads = 0;
        int numWrites = 0;

        // When set, all writes fail
        bool failWrites = false;

    private:
        using Value = std::variant<uint32_t, int32_t, bool>;

        template <typename T>
        T get(const std::string_view key, const T& defaultValue) {
            numReads++;
            const auto it = values.find(key);
            if (it == values.end()) {
                return defaultValue;
            }

            const auto value = std::get_if<T>(&it->second);
            return value != nullptr ? *value : defaultValue;
        }

        template <typename T>
        bool put(const std::string_view key, T value) {
            numWrites++;
            if (failWrites) {
                return false;
            }

            values.insert_or_assign(std::string(key), Value(value));
            return true;
        }

        std::map<std::string, Value, std::less<>> values;
    };
}
