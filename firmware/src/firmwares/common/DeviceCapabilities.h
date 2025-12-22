#pragma once

#include <vector>
#include <Registers/RegisterDescriptor.h>

namespace devices {
    struct DeviceCapabilities {
        std::vector<int> pushButtons;
        std::vector<int> leds;
    };

    struct DeviceCapability {
        virtual ~DeviceCapability() {}
    };

    struct PushButtonCapability final : DeviceCapability {
        PushButtonCapability(uint8_t number, RegisterDescriptor& reg, uint8_t regIndex, int8_t ledIndex)
            : number(number),
              reg(reg),
              regIndex(regIndex),
              ledIndex(ledIndex) {}

        uint8_t number;
        RegisterDescriptor& reg;
        uint8_t regIndex;
        int8_t ledIndex;
    };

    struct RotaryEncoderCapability final : DeviceCapability {
        RotaryEncoderCapability(uint8_t number, RegisterDescriptor& reg, uint8_t aRegIndex, uint8_t bRegIndex)
            : number(number),
              reg(reg),
              aRegIndex(aRegIndex),
              bRegIndex(bRegIndex) {}

        uint8_t number;
        RegisterDescriptor& reg;
        uint8_t aRegIndex;
        uint8_t bRegIndex;
    };
}
