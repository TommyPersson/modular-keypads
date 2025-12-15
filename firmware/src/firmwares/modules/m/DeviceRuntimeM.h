#pragma once

#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/notifications/SwitchStateChangeNotifier.h"
#include "firmwares/common/notifications/EncoderRotationNotifier.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/i2c/I2cPins.h"

#include "RegisterDescriptorsM.h"

namespace devices::m::i2c {
    inline auto pins = ::i2c::Pins{
        .SDA = 2,
        .SCL = 1
    };
}

class DeviceRuntimeM final : public DeviceRuntime {
public:
    explicit DeviceRuntimeM(
        RegisterManager& registers,
        IndicatorLedManager& indicatorLeds,
        Notifier& notifier
    );
    ~DeviceRuntimeM() override;

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;
    std::unique_ptr<EncoderRotationNotifier> encoderRotationNotifier;
};
