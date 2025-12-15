#pragma once
#include <firmwares/common/DeviceConfigurationManager.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/modules/common/DeviceModule.h>

namespace devices::m {
    class DeviceModuleM final : public DeviceModule {
    public:
        DeviceModuleM(
            const DeviceConfiguration& configuration,
            std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
            std::unique_ptr<RegisterManager>& registerManager,
            std::unique_ptr<RegisterRefresher>& registerRefresher,
            std::unique_ptr<DeviceRuntime>& deviceRuntime
        );
        ~DeviceModuleM() override;

        void setup() override;
        void loop() override;

        RegisterManager& getRegisters() override;

    private:
        const DeviceConfiguration configuration;
        std::unique_ptr<IndicatorLedManager> indicatorLedManager;
        std::unique_ptr<RegisterManager> registerManager;
        std::unique_ptr<RegisterRefresher> registerRefresher;
        std::unique_ptr<DeviceRuntime> deviceRuntime;
    };
}
