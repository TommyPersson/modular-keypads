#pragma once

#include <tfw/hal/i2c/Client.h>

#include "Device.h"

namespace mkp::devices::common {
    // A sub-device of a remote device. Commands are sent through the parent's i2c address, while events and
    // identity updates are read by the parent RemoteDevice, as it owns the parent's event endpoint.
    class RemoteSubDevice final : public Device {
    public:
        RemoteSubDevice(
            const DeviceConfiguration& parentConfiguration,
            uint8_t slot,
            tfw::hal::i2c::Client& i2cClient
        );
        ~RemoteSubDevice() override;

        void setup() override;
        void loop() override;

        const DeviceConfiguration& getConfiguration() const override;
        const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const override;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) override;
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) override;
        tfw::utils::void_result rename(const std::string_view& deviceName) override;

        void updateInformation(const i2c::structs::SubDeviceInformation& information);

    private:
        DeviceConfiguration configuration;

        tfw::hal::i2c::Client& i2cClient;
    };
}
