#pragma once

#include <tfw/hal/i2c/Client.h>

#include "Device.h"
#include "RemoteSubDevice.h"

namespace mkp::devices::common {
    class RemoteDevice : public Device {
    public:
        explicit RemoteDevice(
            const DeviceConfiguration& configuration,
            const std::vector<std::shared_ptr<DeviceCapability>>& capabilities,
            tfw::hal::i2c::Client& i2cClient,
            uint8_t numSubDevices = 0
        );
        ~RemoteDevice() override;

        void setup() override;
        void loop() override;

        const DeviceConfiguration& getConfiguration() const override;
        const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const override;
        std::span<Device* const> getSubDevices() const override;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) override;
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) override;
        tfw::utils::void_result rename(const std::string_view& deviceName) override;

        void onRemoteEventsAvailable();

        /**
         * Checks that the device still answers at its address. Returns false when it has failed several checks in
         * a row, after which the count starts over.
         */
        bool checkPresence();

    private:
        void refreshName();
        void refreshSubDevices();

        DeviceConfiguration configuration;

        const std::vector<std::shared_ptr<DeviceCapability>>& capabilities;

        tfw::hal::i2c::Client& i2cClient;
        bool remoteEventsAvailable = true;
        uint8_t numMissedPresenceChecks = 0;

        std::vector<std::unique_ptr<RemoteSubDevice>> subDevices;
        std::vector<Device*> subDevicePointers;
    };
}
