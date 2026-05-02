#pragma once

#include <memory>
#include <firmwares/common/ServiceLocator.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/DeviceLocation.h>

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
                    char deviceName[tfw::utils::i2c::MAX_PACKET_SIZE]{};
                };
#pragma pack(pop)

#pragma pack(push, 1)
                struct DeviceRegisters {
                    // The amount may vary between devices so the actual read-site uses a length override
                    // based on the number of registers.
                    uint8_t data[tfw::utils::i2c::MAX_PACKET_SIZE]{};
                };
#pragma pack(pop)
            }

            namespace endpoints {
                inline tfw::utils::i2c::EndpointDescriptor<structs::DeviceInformation> DeviceInformation{.id = 0x01};
                inline tfw::utils::i2c::EndpointDescriptor<structs::DeviceName> DeviceName{.id = 0x02};
                inline tfw::utils::i2c::EndpointDescriptor<structs::DeviceRegisters> DeviceRegisters{.id = 0x03};
            }
        }
    }

    class DeviceModule {
    public:
        virtual ~DeviceModule() = default;

        virtual void setup() = 0;
        virtual void loop() = 0;

        virtual tfw::utils::registers::RegisterManager& getRegisters() = 0;
        virtual const std::vector<const tfw::utils::registers::RegisterDescriptor*>& getRegisterDescriptors() = 0;
        virtual const DeviceConfiguration& getConfiguration() const = 0;
        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs);
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs);

        tfw::utils::void_result rename(const std::string_view& deviceName);

        tfw::utils::observables::Observable<DeviceSwitchEvent>& onSwitchEvent() { return getRuntime().onSwitchEvent(); }

        tfw::utils::observables::Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() {
            return getRuntime().onRotaryEncoderEvent();
        }

    protected:
        explicit DeviceModule(DeviceLocation deviceLocation, tfw::utils::i2c::Client& i2cClient);

        virtual DeviceRuntime& getRuntime() = 0;

        DeviceLocation deviceLocation;
        tfw::utils::i2c::Client& i2cClient;
    };
}
