#pragma once

#include "mkp/devices/common/LocalDevice.h"
#include "mkp/devices/common/RegisterRefresher.h"

namespace devices::a {
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

    inline const std::vector<std::shared_ptr<DeviceCapability>> capabilities = {
        std::make_shared<PushButtonCapability>(1, registers::IOB, 7, 0),
        std::make_shared<PushButtonCapability>(2, registers::IOA, 0, 11),
        std::make_shared<PushButtonCapability>(3, registers::IOB, 6, 1),
        std::make_shared<PushButtonCapability>(4, registers::IOA, 1, 10),
        std::make_shared<PushButtonCapability>(5, registers::IOB, 5, 2),
        std::make_shared<PushButtonCapability>(6, registers::IOA, 2, 9),
        std::make_shared<PushButtonCapability>(7, registers::IOB, 4, 3),
        std::make_shared<PushButtonCapability>(8, registers::IOA, 3, 8),
        std::make_shared<PushButtonCapability>(9, registers::IOB, 3, 4),
        std::make_shared<PushButtonCapability>(10, registers::IOA, 4, 7),
        std::make_shared<PushButtonCapability>(11, registers::IOB, 2, 5),
        std::make_shared<PushButtonCapability>(12, registers::IOA, 5, 6),
    };

    class LocalDeviceA final : public LocalDevice {
    public:
        LocalDeviceA(
            const DeviceConfiguration& configuration,
            const DeviceConfigurationManager& configurationManager,
            const NotifierFactory& notifierFactory
        );
        ~LocalDeviceA() override;

        void setup() override;
        void loop() override;

        const DeviceConfiguration& getConfiguration() const override { return configuration; }

        const std::vector<std::shared_ptr<devices::DeviceCapability>>& getCapabilities() const override {
            return capabilities;
        }

        const std::vector<const tfw::utils::registers::RegisterDescriptor*>& getRegisterDescriptors() const override {
            return registers::all;
        }

        tfw::hal::i2c::Pins getI2cPins() const override { return i2c::pins; }

    private:
        std::unique_ptr<RegisterRefresher> registerRefresher;
    };
}
