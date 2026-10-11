#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

namespace tfw::hal::i2c {
    namespace addresses {
        // Assigned by the master
        constexpr std::uint8_t PERMANENT_FIRST = 0x10;
        constexpr std::uint8_t PERMANENT_LAST = 0x3f;

        // Picked at random by devices that haven't been assigned a permanent address yet
        constexpr std::uint8_t TEMPORARY_FIRST = 0x40;
        constexpr std::uint8_t TEMPORARY_LAST = 0x77;
    }

    /**
     * Hands out permanent addresses to devices, by device ID. Addresses are remembered, so that a device that
     * restarts gets its previous address back.
     */
    class AddressAllocator {
    public:
        /**
         * Allocates an address for the device, from the ones isAvailable accepts. Prefers the device's previous
         * address, then addresses not remembered for other devices, and then the addresses of devices that are gone.
         */
        std::optional<std::uint8_t> allocate(
            std::uint64_t deviceId,
            const std::function<bool(std::uint8_t)>& isAvailable
        );

        /**
         * Remembers the address of a device found at it, for example after the master restarted. Any other device
         * remembered at the address is forgotten.
         */
        void remember(std::uint64_t deviceId, std::uint8_t address);

        std::optional<std::uint8_t> addressOf(std::uint64_t deviceId) const;

    private:
        bool isRemembered(std::uint8_t address) const;

        std::unordered_map<std::uint64_t, std::uint8_t> addresses;
    };
}
