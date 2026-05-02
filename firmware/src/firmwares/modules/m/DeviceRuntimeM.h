#pragma once

#include "DeviceModuleM.h"
#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/notifications/SwitchStateChangeNotifier.h"
#include "firmwares/common/notifications/EncoderRotationNotifier.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include <tfw/hal/i2c.h>

namespace devices::m::i2c {
    inline auto pins = tfw::utils::i2c::Pins{
        .SDA = 2,
        .SCL = 1
    };
}

class DeviceRuntimeM final : public devices::DeviceRuntime {
public:
    explicit DeviceRuntimeM(
        uint64_t deviceId,
        tfw::utils::registers::RegisterManager& registers,
        IndicatorLedManager& indicatorLeds,
        Notifier& notifier,
        DeviceConfigurationManager& configurationManager
    );
    ~DeviceRuntimeM() override;

    const std::vector<std::shared_ptr<devices::DeviceCapability>>& getCapabilities() const override {
        return devices::m::capabilities;
    }

    const std::vector<const tfw::utils::registers::RegisterDescriptor*>& getRegisterDescriptors() const override {
        return devices::m::registers::all;
    }

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;
    std::unique_ptr<EncoderRotationNotifier> encoderRotationNotifier;
};
