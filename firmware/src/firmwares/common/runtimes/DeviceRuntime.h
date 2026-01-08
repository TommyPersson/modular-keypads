#pragma once

#include <firmwares/common/DeviceCapabilities.h>
#include <firmwares/common/DeviceConfigurationManager.h>
#include <Registers/RegisterManager.h>
#include <utils/result.h>

#include "utils/bitreaders/BitReader.h"
#include "firmwares/common/indicatorleds/IndicatorLedManager.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include "firmwares/common/monitors/RotationalEncoderMonitor.h"
#include "firmwares/common/notifications/Notifier.h"

namespace devices {
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
        : utils::observables::Observer<SwitchEvent>,
          utils::observables::Observer<EncoderRotatedEvent> {
    public:
        explicit DeviceRuntime(
            uint64_t deviceId,
            utils::registers::RegisterManager& registers,
            IndicatorLedManager& indicatorLeds,
            Notifier& notifier,
            DeviceConfigurationManager& configurationManager
        );

        ~DeviceRuntime() override = default;

        virtual void begin();
        virtual void loop();

        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;
        virtual const std::vector<const utils::registers::RegisterDescriptor*>& getRegisterDescriptors() const = 0;

        void flashIdentificationLights(uint32_t durationMs);
        void flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs);
        utils::void_result renameDevice(const std::string_view& deviceName);

        utils::observables::Observable<DeviceSwitchEvent>& onSwitchEvent() { return deviceSwitchEventSubject; }
        utils::observables::Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() { return deviceRotaryEncoderEventSubject; }

    protected:
        void configureRegisters() const;
        void configureCapabilities();

        void attachSwitch(uint8_t number, const std::shared_ptr<utils::bitreaders::BitReader>& bitReader, int8_t ledIndex);
        void attachRotationalEncoder(
            uint8_t number,
            const std::shared_ptr<utils::bitreaders::BitReader>& aBitReader,
            const std::shared_ptr<utils::bitreaders::BitReader>& bBitReader
        );

        void observe(const SwitchEvent& event) override;
        void observe(const EncoderRotatedEvent& event) override;

        std::vector<std::shared_ptr<RotationalEncoderMonitor>> rotationalEncoderMonitors;
        std::vector<std::shared_ptr<SwitchMonitor>> switchMonitors;

        utils::observables::Subject<DeviceSwitchEvent> deviceSwitchEventSubject;
        utils::observables::Subject<DeviceRotaryEncoderEvent> deviceRotaryEncoderEventSubject;

        uint64_t deviceId;
        utils::registers::RegisterManager& registers;
        IndicatorLedManager& indicatorLeds;
        Notifier& notifier;
        DeviceConfigurationManager& configurationManager;
    };
}
