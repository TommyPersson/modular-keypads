#pragma once

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/indicatorleds/IndicatorLeds.h"

#include <MCP23x17/MCP23x17.h>
#include <SerialPort/SerialPort.h>

class FirmwareModuleA final : public Firmware {
public:
    explicit FirmwareModuleA(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Notifier& notifier,
        Logger& logger
    );

    ~FirmwareModuleA() override;

    void setup() override;
    void loop() override;

private:
    void attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, uint8_t ledIndex);

    std::unique_ptr<MCP23x17> mcp23x17;
    std::unique_ptr<IndicatorLedManager> indicatorLeds;

    std::vector<std::shared_ptr<SwitchMonitor>> switchMonitors;
};
