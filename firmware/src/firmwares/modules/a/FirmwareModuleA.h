#pragma once

#include "firmwares/Firmware.h"
#include "firmwares/common/i2c/SlavePort.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/runtimes/RegisterRefresher.h"

#include <SerialPort/SerialPort.h>

class FirmwareModuleA final : public Firmware {
public:
    explicit FirmwareModuleA(ServiceLocator& serviceLocator);

    ~FirmwareModuleA() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<IndicatorLedManager> indicatorLeds;
    std::unique_ptr<i2c::SlavePort> i2cSlavePort;

    std::unique_ptr<RegisterRefresher> registerRefresher;
    std::unique_ptr<DeviceRuntime> runtime;
};
