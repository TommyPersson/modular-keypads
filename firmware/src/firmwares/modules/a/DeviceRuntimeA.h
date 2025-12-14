#pragma once

#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/notifications/SwitchStateChangeNotifier.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "RegisterDescriptorsA.h"

class DeviceRuntimeA final : public DeviceRuntime {
public:
    explicit DeviceRuntimeA(
        RegisterManager& registers,
        IndicatorLedManager& indicatorLeds,
        Notifier& notifier,
        DeviceMode mode
    );
    ~DeviceRuntimeA() override;

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;
};
