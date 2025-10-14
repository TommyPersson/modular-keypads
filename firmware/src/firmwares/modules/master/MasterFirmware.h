#pragma once

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/indicatorleds/IndicatorLeds.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include "firmwares/common/notifications/SwitchStateChangeNotifier.h"

#include <MCP23x17/MCP23x17.h>
#include <SerialPort/SerialPort.h>

class MasterFirmware final : public Firmware {
public:
    explicit MasterFirmware(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Notifier& notifier,
        Logger& logger
    );

    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

private:
    void attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, int ledIndex);

    std::unique_ptr<MCP23x17> mcp23x17;
    std::unique_ptr<IndicatorLedManager> indicatorLeds;
    std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;

    std::vector<std::shared_ptr<SwitchMonitor>> switchMonitors;

    std::unique_ptr<OutputPin> mcpResetPin;
};
