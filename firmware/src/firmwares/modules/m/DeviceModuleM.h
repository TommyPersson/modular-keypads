#pragma once

#include <firmwares/common/DeviceCapabilities.h>
#include <firmwares/common/DeviceConfigurationManager.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/modules/common/DeviceModule.h>

namespace devices::m {
    namespace registers {
        const utils::registers::RegisterDescriptor IOA{.name = "IOA", .index = 0};
        const utils::registers::RegisterDescriptor IOB{.name = "IOB", .index = 1};
        const inline std::vector all = {&IOA, &IOB};
    }

    inline const std::vector<std::shared_ptr<DeviceCapability>> capabilities = {
        std::make_shared<PushButtonCapability>(1, registers::IOB, 0, 0),
        std::make_shared<PushButtonCapability>(2, registers::IOB, 1, 1),
        std::make_shared<PushButtonCapability>(3, registers::IOB, 2, 2),
        std::make_shared<PushButtonCapability>(4, registers::IOB, 3, 3),
        std::make_shared<PushButtonCapability>(5, registers::IOB, 4, -1),
        std::make_shared<RotaryEncoderCapability>(1, registers::IOA, 0, 1),
    };

    class DeviceModuleM final : public DeviceModule {
    public:
        DeviceModuleM(
            DeviceMode deviceMode,
            const DeviceConfiguration& configuration,
            std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
            std::unique_ptr<utils::registers::RegisterManager>& registerManager,
            std::unique_ptr<RegisterRefresher>& registerRefresher,
            std::unique_ptr<DeviceRuntime>& deviceRuntime,
            std::unique_ptr<Notifier>& notifier,
            utils::i2c::Client& i2cClient
        );
        ~DeviceModuleM() override;

        void setup() override;
        void loop() override;

        utils::registers::RegisterManager& getRegisters() override;

        const std::vector<const utils::registers::RegisterDescriptor*>& getRegisterDescriptors() override {
            return registers::all;
        }

        const DeviceConfiguration& getConfiguration() const override { return configuration; }

        const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const override {
            return capabilities;
        }

        void flashIdentificationLights(uint32_t duration_ms) override;

    protected:
        DeviceRuntime& getRuntime() override {
            return *deviceRuntime;
        }

    private:
        const DeviceMode deviceMode;
        const DeviceConfiguration configuration;
        std::unique_ptr<IndicatorLedManager> indicatorLedManager;
        std::unique_ptr<utils::registers::RegisterManager> registerManager;
        std::unique_ptr<RegisterRefresher> registerRefresher;
        std::unique_ptr<DeviceRuntime> deviceRuntime;
        std::unique_ptr<Notifier> notifier;
        utils::i2c::Client& i2cClient;
    };
}
