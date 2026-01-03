#pragma once

#include <memory>
#include <firmwares/common/ServiceLocator.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>

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
                    char deviceName[utils::i2c::MAX_PACKET_SIZE]{};
                };
#pragma pack(pop)

#pragma pack(push, 1)
                struct DeviceRegisters {
                    // TODO make the size device dependent, the size has a large impact on the refresh time,
                    // TODO so keeping it as small as possible for each device is important
                    uint8_t data[4]{};
                    //uint8_t data[utils::i2c::MAX_PACKET_SIZE]{};
                };
#pragma pack(pop)
            }

            namespace endpoints {
                inline utils::i2c::EndpointDescriptor<structs::DeviceInformation> DeviceInformation{.id = 0x01};
                inline utils::i2c::EndpointDescriptor<structs::DeviceName> DeviceName{.id = 0x02};
                inline utils::i2c::EndpointDescriptor<structs::DeviceRegisters> DeviceRegisters{.id = 0x03};
            }
        }
    }

    class DeviceModule {
    public:
        virtual ~DeviceModule() = default;

        virtual void setup() = 0;
        virtual void loop() = 0;

        virtual utils::registers::RegisterManager& getRegisters() = 0;
        virtual const std::vector<const utils::registers::RegisterDescriptor*>& getRegisterDescriptors() = 0;
        virtual const DeviceConfiguration& getConfiguration() const = 0;
        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;

        utils::observables::Observable<DeviceSwitchEvent>& onSwitchEvent() { return getRuntime().onSwitchEvent(); };

        utils::observables::Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() {
            return getRuntime().onRotaryEncoderEvent();
        };

        static std::unique_ptr<DeviceModule> local(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        );

        static std::unique_ptr<DeviceModule> remote(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        );

    protected:
        DeviceModule() = default;

        virtual DeviceRuntime& getRuntime() = 0;
    };
}
