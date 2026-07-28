#pragma once

#include <tfw/hal/i2c/Client.h>

#include "Device.h"
#include "../../../firmwares/common/notifications/NotifierFactory.h"
#include "../../../firmwares/common/notifications/SwitchStateChangeNotifier.h"

namespace devices {
    class RemoteDevice : public Device {
    public:
        explicit RemoteDevice(
            const DeviceConfiguration& configuration,
            const NotifierFactory& notifierFactory,
            tfw::hal::i2c::Client& i2cClient
        );
        ~RemoteDevice() override;

        void setup() override;
        void loop() override;

        const DeviceConfiguration& getConfiguration() const override;
        const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const override;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) override;
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) override;
        tfw::utils::void_result rename(const std::string_view& deviceName) override;

    private:
        DeviceConfiguration configuration;
        std::unique_ptr<Notifier> notifier;
        std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;

        tfw::hal::i2c::Client& i2cClient;
    };
}
