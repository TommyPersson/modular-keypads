#pragma once

#include <firmwares/common/DeviceCapabilities.h>
#include <Registers/RegisterManager.h>

#include "utils/bitreaders/BitReader.h"
#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include "firmwares/common/monitors/RotationalEncoderMonitor.h"
#include "firmwares/common/notifications/Notifier.h"

namespace devices {
    enum class DeviceMode {
        Local,
        Remote
    };

    struct DeviceSwitchEvent {
        const uint64_t deviceId;
        const uint8_t switchNumber;
        const SwitchState state;
    };

    struct DeviceRotaryEncoderEvent {
        const uint64_t deviceId;
        const uint8_t encoderNumber;
        const RotationalEncoderDirection direction;
    };

    class DeviceRuntime
        : Observer<SwitchEvent>,
          Observer<EncoderRotatedEvent> {
    public:
        explicit DeviceRuntime(
            uint64_t deviceId,
            RegisterManager& registers,
            IndicatorLedManager& indicatorLeds,
            Notifier& notifier
        );

        ~DeviceRuntime() override = default;

        virtual void begin();
        virtual void loop();

        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;
        virtual const std::vector<const RegisterDescriptor*>& getRegisterDescriptors() const = 0;

        Observable<DeviceSwitchEvent>& onSwitchEvent() { return deviceSwitchEventSubject; }
        Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() { return deviceRotaryEncoderEventSubject; }

    protected:
        void configureRegisters() const;
        void configureCapabilities();

        void attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, int8_t ledIndex);
        void attachRotationalEncoder(
            uint8_t number,
            const std::shared_ptr<BitReader>& aBitReader,
            const std::shared_ptr<BitReader>& bBitReader
        );

        void observe(const SwitchEvent& event) override;
        void observe(const EncoderRotatedEvent& event) override;

        std::vector<std::shared_ptr<RotationalEncoderMonitor>> rotationalEncoderMonitors;
        std::vector<std::shared_ptr<SwitchMonitor>> switchMonitors;

        Subject<DeviceSwitchEvent> deviceSwitchEventSubject;
        Subject<DeviceRotaryEncoderEvent> deviceRotaryEncoderEventSubject;

        uint64_t deviceId;
        RegisterManager& registers;
        IndicatorLedManager& indicatorLeds;
        Notifier& notifier;
    };
}
