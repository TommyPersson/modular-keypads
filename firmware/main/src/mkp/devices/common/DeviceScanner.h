#pragma once

#include <memory>

#include "DeviceConfigurationManager.h"
#include <tfw/hal/i2c.h>
#include <tfw/hal/time.h>

namespace mkp::devices::common {
    class DeviceScanner {
    public:
        explicit DeviceScanner(tfw::hal::i2c::Client& client, tfw::hal::time::Clock& clock);
        ~DeviceScanner();

        /**
         * Assigns permanent addresses to the devices waiting at temporary ones, and then finds all devices at
         * permanent addresses. Addresses are remembered across scans, so that devices that restart are assigned
         * their previous address again.
         */
        std::vector<std::shared_ptr<DeviceConfiguration>> scan();

        /// Probes all permanent and temporary addresses, for diagnostics
        std::vector<uint8_t> findRespondingAddresses();

    private:
        struct AssignmentRound {
            int numAssigned = 0;
            int numCollisions = 0;
        };

        /**
         * Moves the devices at temporary addresses to permanent ones. Devices whose temporary addresses collided are
         * made to pick new ones, and are assigned in the next round.
         */
        void assignAddresses();
        AssignmentRound assignAddressesOnce();

        std::vector<std::shared_ptr<DeviceConfiguration>> scanPermanentAddresses();

        tfw::hal::i2c::Client& client;
        tfw::hal::time::Clock& clock;
        tfw::hal::i2c::AddressAllocator addressAllocator;
    };
}
