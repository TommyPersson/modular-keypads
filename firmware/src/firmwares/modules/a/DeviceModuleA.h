#pragma once

#include <firmwares/common/DeviceConfigurationManager.h>
#include <firmwares/common/DeviceCapabilities.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/modules/common/DeviceModule.h>

namespace devices::a {
    namespace registers {
        const utils::registers::RegisterDescriptor IOA{.name = "IOA", .index = 0};
        const utils::registers::RegisterDescriptor IOB{.name = "IOB", .index = 1};
        const inline std::vector all = {&IOA, &IOB};
    }

    inline const std::vector<std::shared_ptr<DeviceCapability>> capabilities = {
        std::make_shared<PushButtonCapability>(1, registers::IOB, 0, 0),
        std::make_shared<PushButtonCapability>(2, registers::IOB, 1, 11),
        std::make_shared<PushButtonCapability>(3, registers::IOB, 2, 1),
        std::make_shared<PushButtonCapability>(4, registers::IOB, 3, 10),
        std::make_shared<PushButtonCapability>(5, registers::IOB, 4, 2),
        std::make_shared<PushButtonCapability>(6, registers::IOB, 5, 9),
        std::make_shared<PushButtonCapability>(7, registers::IOB, 6, 3),
        std::make_shared<PushButtonCapability>(8, registers::IOB, 7, 8),
        std::make_shared<PushButtonCapability>(9, registers::IOA, 0, 4),
        std::make_shared<PushButtonCapability>(10, registers::IOA, 1, 7),
        std::make_shared<PushButtonCapability>(11, registers::IOA, 2, 5),
        std::make_shared<PushButtonCapability>(12, registers::IOA, 3, 6),
    };

    class DeviceModuleA final : public DeviceModule {
    public:
        DeviceModuleA(
            const DeviceMode deviceMode,
            const DeviceConfiguration& configuration,
            std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
            std::unique_ptr<utils::registers::RegisterManager>& registerManager,
            std::unique_ptr<RegisterRefresher>& registerRefresher,
            std::unique_ptr<DeviceRuntime>& deviceRuntime,
            std::unique_ptr<Notifier>& notifier,
            utils::i2c::Client& i2cClient
        );
        ~DeviceModuleA() override;

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

        void flashIdentificationLights(uint32_t durationMs) override;

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
