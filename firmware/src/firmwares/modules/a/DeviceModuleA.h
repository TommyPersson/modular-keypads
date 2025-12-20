#pragma once

#include <firmwares/common/DeviceConfigurationManager.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/modules/common/DeviceModule.h>

namespace devices::a {
    class DeviceModuleA final : public DeviceModule {
    public:
        DeviceModuleA(
            const DeviceConfiguration& configuration,
            std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
            std::unique_ptr<RegisterManager>& registerManager,
            std::unique_ptr<RegisterRefresher>& registerRefresher,
            std::unique_ptr<DeviceRuntime>& deviceRuntime,
            std::unique_ptr<Notifier>& notifier
        );
        ~DeviceModuleA() override;

        void setup() override;
        void loop() override;

        RegisterManager& getRegisters() override;

    protected:
        DeviceRuntime& getRuntime() override {
            return *deviceRuntime;
        };

    private:
        const DeviceConfiguration configuration;
        std::unique_ptr<IndicatorLedManager> indicatorLedManager;
        std::unique_ptr<RegisterManager> registerManager;
        std::unique_ptr<RegisterRefresher> registerRefresher;
        std::unique_ptr<DeviceRuntime> deviceRuntime;
        std::unique_ptr<Notifier> notifier;
    };
}
