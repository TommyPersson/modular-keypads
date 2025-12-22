#pragma once

#include <firmwares/common/DeviceCapabilities.h>
#include <firmwares/common/DeviceConfigurationManager.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/modules/common/DeviceModule.h>

namespace devices::m {
    namespace registers {
        const RegisterDescriptor IOA{.name = "IOA", .index = 0};
        const RegisterDescriptor IOB{.name = "IOB", .index = 1};
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
            const DeviceConfiguration& configuration,
            std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
            std::unique_ptr<RegisterManager>& registerManager,
            std::unique_ptr<RegisterRefresher>& registerRefresher,
            std::unique_ptr<DeviceRuntime>& deviceRuntime,
            std::unique_ptr<Notifier>& notifier
        );
        ~DeviceModuleM() override;

        void setup() override;
        void loop() override;

        RegisterManager& getRegisters() override;

        const std::vector<const RegisterDescriptor*>& getRegisterDescriptors() override { return registers::all; }

        const DeviceConfiguration& getConfiguration() const override { return configuration; }

        const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const override {
            return capabilities;
        }

    protected:
        DeviceRuntime& getRuntime() override {
            return *deviceRuntime;
        }

    private:
        const DeviceConfiguration configuration;
        std::unique_ptr<IndicatorLedManager> indicatorLedManager;
        std::unique_ptr<RegisterManager> registerManager;
        std::unique_ptr<RegisterRefresher> registerRefresher;
        std::unique_ptr<DeviceRuntime> deviceRuntime;
        std::unique_ptr<Notifier> notifier;
    };
}
