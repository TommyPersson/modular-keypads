#pragma once

#include "./DeviceRuntimeA.h"
#include "./LocalRegisterRefresherA.h"

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/i2c/SlavePort.h"
#include "firmwares/common/indicatorleds/IndicatorLeds.h"
#include "firmwares/common/logging/Logger.h"

#include <SerialPort/SerialPort.h>

class FirmwareModuleA final : public Firmware {
public:
    explicit FirmwareModuleA(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Notifier& notifier,
        Logger& logger,
        TwoWire& i2c
    );

    ~FirmwareModuleA() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<IndicatorLedManager> indicatorLeds;
    std::unique_ptr<i2c::SlavePort> i2cSlavePort;

    std::unique_ptr<RegisterRefresher> registerRefresher;
    std::unique_ptr<DeviceRuntime> runtime;
};
