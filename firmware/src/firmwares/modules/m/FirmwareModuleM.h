#pragma once

#include <SerialPort/SerialPort.h>

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/notifications/EncoderRotationNotifier.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/runtimes/RegisterRefresher.h"

class FirmwareModuleM final : public Firmware {
public:
    explicit FirmwareModuleM(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Notifier& notifier,
        Logger& logger,
        TwoWire& i2c
    );

    ~FirmwareModuleM() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<IndicatorLedManager> indicatorLeds;
    std::unique_ptr<RegisterRefresher> registerRefresher;
    std::unique_ptr<DeviceRuntime> runtime;
};
