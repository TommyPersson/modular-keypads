#pragma once

#include <memory>
#include <firmwares/common/ServiceLocator.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>

namespace devices {
    class DeviceModule {
    public:
        virtual ~DeviceModule() = default;

        virtual void setup() = 0;
        virtual void loop() = 0;

        virtual utils::registers::RegisterManager& getRegisters() = 0;
        virtual const std::vector<const utils::registers::RegisterDescriptor*>& getRegisterDescriptors() = 0;
        virtual const DeviceConfiguration& getConfiguration() const = 0;
        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;

        utils::observables::Observable<DeviceSwitchEvent>& onSwitchEvent() { return getRuntime().onSwitchEvent(); };
        utils::observables::Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() { return getRuntime().onRotaryEncoderEvent(); };

        static std::unique_ptr<DeviceModule> local(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        );

        static std::unique_ptr<DeviceModule> remote(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        );

    protected:
        DeviceModule() = default;

        virtual DeviceRuntime& getRuntime() = 0;
    };
}