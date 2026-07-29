#pragma once

#include "mkp/devices/common/LocalDevice.h"
#include "mkp/devices/common/RegisterRefresher.h"

namespace mkp::devices::a {
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
        std::make_shared<common::PushButtonCapability>(1, registers::IOB, 7, 0),
        std::make_shared<common::PushButtonCapability>(2, registers::IOA, 0, 11),
        std::make_shared<common::PushButtonCapability>(3, registers::IOB, 6, 1),
        std::make_shared<common::PushButtonCapability>(4, registers::IOA, 1, 10),
        std::make_shared<common::PushButtonCapability>(5, registers::IOB, 5, 2),
        std::make_shared<common::PushButtonCapability>(6, registers::IOA, 2, 9),
        std::make_shared<common::PushButtonCapability>(7, registers::IOB, 4, 3),
        std::make_shared<common::PushButtonCapability>(8, registers::IOA, 3, 8),
        std::make_shared<common::PushButtonCapability>(9, registers::IOB, 3, 4),
        std::make_shared<common::PushButtonCapability>(10, registers::IOA, 4, 7),
        std::make_shared<common::PushButtonCapability>(11, registers::IOB, 2, 5),
        std::make_shared<common::PushButtonCapability>(12, registers::IOA, 5, 6),
    };

    class LocalDeviceA final : public common::LocalDevice {
    public:
        LocalDeviceA(
            const common::DeviceConfiguration& configuration,
            const common::DeviceConfigurationManager& configurationManager,
            const components::notifications::NotifierFactory& notifierFactory
        );
        ~LocalDeviceA() override;

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

    private:
        std::unique_ptr<common::RegisterRefresher> registerRefresher;
    };
}
