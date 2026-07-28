#pragma once

#include <tfw/hal/i2c/Client.h>

#include "runtimes/DeviceRuntime.h"

namespace devices {
    class Device {
    public:
        virtual ~Device() = default;

        virtual void setup() = 0;
        virtual void loop() = 0;

        virtual const DeviceConfiguration& getConfiguration() const = 0;
        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;

        virtual tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) = 0;
        virtual tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) = 0;
        virtual tfw::utils::void_result rename(const std::string_view& deviceName) = 0;

        tfw::utils::observables::Observable<DeviceSwitchEvent>& onSwitchEvent() { return deviceSwitchEventSubject; }

        tfw::utils::observables::Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() {
            return deviceRotaryEncoderEventSubject;
        }

    protected:
        tfw::utils::observables::Subject<DeviceSwitchEvent> deviceSwitchEventSubject;
        tfw::utils::observables::Subject<DeviceRotaryEncoderEvent> deviceRotaryEncoderEventSubject;
    };
}
