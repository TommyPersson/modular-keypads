#pragma once

#include <memory>

#include <tfw/hal/time.h>

#include "Device.h"
#include "mkp/components/leds/IndicatorLedManager.h"

namespace mkp::devices::common {
    class LocalDevice :
        public Device,
        public tfw::utils::observables::Observer<DeviceSwitchEvent>,
        public tfw::utils::observables::Observer<DeviceRotaryEncoderEvent>,
        public tfw::utils::observables::Observer<tfw::hal::buttons::ButtonStateChangedEvent>,
        public tfw::utils::observables::Observer<tfw::hal::encoders::EncoderRotatedEvent> {
    public:
        void setup() override;
        void loop() override;

        tfw::utils::registers::RegisterManager& getRegisters() const;
        virtual const std::vector<const tfw::utils::registers::RegisterDescriptor*>& getRegisterDescriptors() const = 0;
        virtual tfw::hal::i2c::Pins getI2cPins() const = 0;
        virtual uint8_t getEventInterruptPin() const = 0;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) override;
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) override;
        tfw::utils::void_result rename(const std::string_view& deviceName) override;

        void observe(const DeviceSwitchEvent& event) override;
        void observe(const DeviceRotaryEncoderEvent& event) override;

        void observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) override;
        void observe(const tfw::hal::encoders::EncoderRotatedEvent& event) override;
    protected:
        explicit LocalDevice(
            const DeviceConfiguration& configuration,
            const DeviceConfigurationManager& configurationManager,
            std::unique_ptr<components::leds::IndicatorLedManager>&& indicatorLedManager,
            tfw::hal::time::Clock& clock
        );

        void setupRegisters() const;
        void setupCapabilities();

        void attachSwitch(
            uint8_t number,
            const std::shared_ptr<tfw::hal::bitreaders::BitReader>& bitReader,
            int8_t ledIndex
        );
        void attachRotationalEncoder(
            uint8_t number,
            const std::shared_ptr<tfw::hal::bitreaders::BitReader>& aBitReader,
            const std::shared_ptr<tfw::hal::bitreaders::BitReader>& bBitReader
        );

        const DeviceConfiguration configuration;
        const DeviceConfigurationManager& configurationManager;
        tfw::hal::time::Clock& clock;
        std::unique_ptr<tfw::utils::registers::RegisterManager> registerManager;
        std::unique_ptr<components::leds::IndicatorLedManager> indicatorLedManager;

        std::vector<std::shared_ptr<tfw::hal::encoders::RotaryEncoder>> rotaryEncoders;
        std::vector<std::shared_ptr<tfw::hal::buttons::Button>> buttons;
    };
}
