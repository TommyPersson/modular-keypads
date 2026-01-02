#pragma once

#include "DeviceModuleA.h"
#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/notifications/SwitchStateChangeNotifier.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/i2c/I2cPins.h"

namespace devices::a::i2c {
    inline auto pins = ::i2c::Pins{
        .SDA = 1,
        .SCL = 0
    };
}

class DeviceRuntimeA final : public devices::DeviceRuntime {
public:
    explicit DeviceRuntimeA(
        uint64_t deviceId,
        utils::registers::RegisterManager& registers,
        IndicatorLedManager& indicatorLeds,
        Notifier& notifier
    );
    ~DeviceRuntimeA() override;

    const std::vector<std::shared_ptr<devices::DeviceCapability>>& getCapabilities() const override {
        return devices::a::capabilities;
    }

    const std::vector<const utils::registers::RegisterDescriptor*>& getRegisterDescriptors() const override {
        return devices::a::registers::all;
    }

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<SwitchStateChangeNotifier> switchStateChangeNotifier;
};
