#pragma once

#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/notifications/SwitchStateChangeNotifier.h"
#include "firmwares/common/notifications/EncoderRotationNotifier.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"

#include "RegisterDescriptorsM.h"

class DeviceRuntimeM final : public DeviceRuntime {
public:
    explicit DeviceRuntimeM(
        RegisterManager& registers,
        IndicatorLedManager& indicatorLeds,
        Notifier& notifier,
        DeviceMode mode
    );
    ~DeviceRuntimeM() override;

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;
    std::unique_ptr<EncoderRotationNotifier> encoderRotationNotifier;
};
