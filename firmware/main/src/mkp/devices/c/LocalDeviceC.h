#pragma once

#include "mkp/devices/common/LocalDevice.h"
#include "mkp/devices/common/RegisterRefresher.h"
#include <tfw/hal/time.h>

#include "subdevices/SubDevice.h"
#include "tfw/hal/spi/AndroidSPIBus.h"

namespace mkp::devices::c {
    namespace i2c {
        inline auto pins = tfw::hal::i2c::Pins{
            .SDA = 11,
            .SCL = 10
        };
    }

    namespace registers {
        const tfw::utils::registers::RegisterDescriptor IOA{.name = "IOA", .index = 0};
        const tfw::utils::registers::RegisterDescriptor IOB{.name = "IOB", .index = 1};
        const inline std::vector all = {&IOA, &IOB};
    }

    inline const std::vector<std::shared_ptr<common::DeviceCapability>> capabilities = {
    };

    class LocalDeviceC final
        : public common::LocalDevice,
          public tfw::utils::observables::Observer<SubDeviceConnectedEvent>,
          public tfw::utils::observables::Observer<SubDeviceDisconnectedEvent> {
    public:
        LocalDeviceC(
            const common::DeviceConfiguration& configuration,
            const common::DeviceConfigurationManager& configurationManager,
            const components::notifications::NotifierFactory& notifierFactory,
            tfw::hal::time::Clock& clock
        );
        ~LocalDeviceC() override;

        void setup() override;
        void loop() override;

        const common::DeviceConfiguration& getConfiguration() const override { return configuration; }

        const std::vector<std::shared_ptr<common::DeviceCapability>>& getCapabilities() const override {
            return capabilities;
        }

        const std::vector<const tfw::utils::registers::RegisterDescriptor*>& getRegisterDescriptors() const override {
            return registers::all;
        }

        tfw::hal::i2c::Pins getI2cPins() const override { return i2c::pins; }
        uint8_t getEventInterruptPin() const override { return 21; }

        void observe(const SubDeviceConnectedEvent& event) override;
        void observe(const SubDeviceDisconnectedEvent& event) override;

    private:
        std::unique_ptr<SubDevice> subDevice1;
        std::unique_ptr<SubDevice> subDevice2;
        std::unique_ptr<SubDevice> subDevice3;

        std::unique_ptr<common::RegisterRefresher> registerRefresher;
        std::unique_ptr<tfw::hal::gpio::OutputPin> pinModReset;
        std::shared_ptr<tfw::hal::gpio::InputPin> pinModMiso;
        std::shared_ptr<tfw::hal::gpio::OutputPin> pinModMosi;
        std::shared_ptr<tfw::hal::gpio::OutputPin> pinModSck;
        std::shared_ptr<tfw::hal::gpio::OutputPin> pinModCs1;
        std::shared_ptr<tfw::hal::gpio::OutputPin> pinModCs2;
        std::shared_ptr<tfw::hal::gpio::OutputPin> pinModCs3;
        std::shared_ptr<tfw::hal::gpio::InputPin> pinMod1Int;
        std::shared_ptr<tfw::hal::gpio::InputPin> pinMod2Int;
        std::shared_ptr<tfw::hal::gpio::InputPin> pinMod3Int;
        std::unique_ptr<SPIClass> spi;
    };
}
