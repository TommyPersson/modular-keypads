#pragma once

#include <tfw/hal/i2c/Client.h>

#include "Device.h"
#include "mkp/components/notifications/NotifierFactory.h"
#include "mkp/components/notifications/SwitchStateChangeNotifier.h"

namespace mkp::devices::common {
    class RemoteDevice : public Device {
    public:
        explicit RemoteDevice(
            const DeviceConfiguration& configuration,
            const NotifierFactory& notifierFactory,
            const std::vector<std::shared_ptr<DeviceCapability>>& capabilities,
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

        const std::vector<std::shared_ptr<DeviceCapability>>& capabilities;

        tfw::hal::i2c::Client& i2cClient;
    };
}
