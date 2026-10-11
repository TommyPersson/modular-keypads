#include "AddressAllocator.h"

#include <algorithm>

namespace tfw::hal::i2c {
    std::optional<std::uint8_t> AddressAllocator::allocate(
        const std::uint64_t deviceId,
        const std::function<bool(std::uint8_t)>& isAvailable
    ) {
        if (const auto previous = addressOf(deviceId); previous.has_value() && isAvailable(*previous)) {
            return previous;
        }

        std::optional<std::uint8_t> fallback;
        for (int address = addresses::PERMANENT_FIRST; address <= addresses::PERMANENT_LAST; address++) {
            if (!isAvailable(address)) {
                continue;
            }

            if (!isRemembered(address)) {
                remember(deviceId, address);
                return address;
            }

            if (!fallback.has_value()) {
                fallback = address;
            }
        }

        if (fallback.has_value()) {
            remember(deviceId, *fallback);
        }

        return fallback;
    }

    void AddressAllocator::remember(const std::uint64_t deviceId, const std::uint8_t address) {
        std::erase_if(addresses, [&](const auto& entry) {
            return entry.first != deviceId && entry.second == address;
        });

        addresses[deviceId] = address;
    }

    std::optional<std::uint8_t> AddressAllocator::addressOf(const std::uint64_t deviceId) const {
        const auto it = addresses.find(deviceId);
        if (it == addresses.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    bool AddressAllocator::isRemembered(const std::uint8_t address) const {
        return std::ranges::any_of(addresses, [&](const auto& entry) { return entry.second == address; });
    }
}
