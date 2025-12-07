#pragma once

#include <Registers/RegisterManager.h>

#include "firmwares/common/bitreaders/BitReader.h"
#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include "firmwares/common/notifications/Notifier.h"

enum class DeviceMode {
    Local,
    Remote
};

class DeviceRuntime {
public:
    explicit DeviceRuntime(
        RegisterManager& registers,
        IndicatorLedManager& indicatorLeds,
        Notifier& notifier,
        Logger& logger,
        DeviceMode mode
    );

    virtual ~DeviceRuntime() = default;

    virtual void begin();
    virtual void loop();

protected:
    std::shared_ptr<Register> addRegister(const std::string& name) const;

    void attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, uint8_t ledIndex);

    std::vector<std::shared_ptr<SwitchMonitor>> switchMonitors;

    RegisterManager& registers;
    IndicatorLedManager& indicatorLeds;
    Notifier& notifier;
    Logger& logger;
    DeviceMode mode;
};
