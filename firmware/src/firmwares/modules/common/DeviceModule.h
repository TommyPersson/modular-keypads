#pragma once

#include <memory>
#include <firmwares/common/ServiceLocator.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/DeviceLocation.h>

#include "firmwares/common/Device.h"

namespace devices {
    namespace common {
        namespace i2c {
            namespace structs {
#pragma pack(push, 1)
                struct DeviceInformation {
                    uint64_t deviceId = 0;
                    char deviceType = 'g';
                };
#pragma pack(pop)

#pragma pack(push, 1)
                struct DeviceName {
                    char deviceName[tfw::hal::i2c::MAX_PACKET_SIZE]{};
                };
#pragma pack(pop)

#pragma pack(push, 1)
                struct DeviceRegisters {
                    // The amount may vary between devices so the actual read-site uses a length override
                    // based on the number of registers.
                    uint8_t data[tfw::hal::i2c::MAX_PACKET_SIZE]{};
                };
#pragma pack(pop)
            }

            namespace endpoints {
                inline tfw::hal::i2c::EndpointDescriptor<structs::DeviceInformation> DeviceInformation{.id = 0x01};
                inline tfw::hal::i2c::EndpointDescriptor<structs::DeviceName> DeviceName{.id = 0x02};
                inline tfw::hal::i2c::EndpointDescriptor<structs::DeviceRegisters> DeviceRegisters{.id = 0x03};
            }
        }
    }

    class DeviceModule :
        public Device,
        tfw::utils::observables::Observer<DeviceSwitchEvent>,
        tfw::utils::observables::Observer<DeviceRotaryEncoderEvent> {
    public:
        void setup() override;
        void loop() override;

        virtual tfw::utils::registers::RegisterManager& getRegisters() = 0;
        virtual const std::vector<const tfw::utils::registers::RegisterDescriptor*>& getRegisterDescriptors() = 0;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) override;
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) override;
        tfw::utils::void_result rename(const std::string_view& deviceName) override;

        void observe(const DeviceSwitchEvent& event) override;
        void observe(const DeviceRotaryEncoderEvent& event) override;

    protected:
        explicit DeviceModule(
            const DeviceConfiguration& configuration
        );

        virtual DeviceRuntime& getRuntime() = 0;

        const DeviceConfiguration configuration;
    };
}
