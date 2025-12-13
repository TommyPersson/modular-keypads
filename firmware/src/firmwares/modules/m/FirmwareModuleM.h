#pragma once

#include "firmwares/Firmware.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/runtimes/RegisterRefresher.h"

class FirmwareModuleM final : public Firmware {
public:
    explicit FirmwareModuleM(ServiceLocator& serviceLocator);

    ~FirmwareModuleM() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<IndicatorLedManager> indicatorLeds;
    std::unique_ptr<RegisterRefresher> registerRefresher;
    std::unique_ptr<DeviceRuntime> runtime;
};
