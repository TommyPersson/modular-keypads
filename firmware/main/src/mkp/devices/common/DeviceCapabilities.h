#pragma once

#include <vector>
#include <tfw/utils/registers.h>

namespace mkp::devices::common {
    struct DeviceCapabilities {
        std::vector<int> pushButtons;
        std::vector<int> leds;
    };

    // Placeholder for capabilities of devices that don't expose their inputs as local registers
    inline const tfw::utils::registers::RegisterDescriptor NoRegister{.name = "", .index = 0};

    struct DeviceCapability {
        virtual ~DeviceCapability() = default;
    };

    struct PushButtonCapability final : DeviceCapability {
        PushButtonCapability(uint8_t number, const tfw::utils::registers::RegisterDescriptor& reg, uint8_t regIndex, int8_t ledIndex)
            : number(number),
              reg(reg),
              regIndex(regIndex),
              ledIndex(ledIndex) {}

        uint8_t number;
        const tfw::utils::registers::RegisterDescriptor& reg;
        uint8_t regIndex;
        int8_t ledIndex;
    };

    struct RotaryEncoderCapability final : DeviceCapability {
        RotaryEncoderCapability(uint8_t number, const tfw::utils::registers::RegisterDescriptor& reg, uint8_t aRegIndex, uint8_t bRegIndex)
            : number(number),
              reg(reg),
              aRegIndex(aRegIndex),
              bRegIndex(bRegIndex) {}

        uint8_t number;
        const tfw::utils::registers::RegisterDescriptor& reg;
        uint8_t aRegIndex;
        uint8_t bRegIndex;
    };
}
