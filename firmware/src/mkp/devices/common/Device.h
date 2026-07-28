#pragma once

#include <tfw/hal/i2c/Client.h>

#include "DeviceCapabilities.h"
#include "DeviceConfigurationManager.h"
#include "tfw/hal/buttons/Button.h"
#include "tfw/hal/encoders/RotaryEncoder.h"

namespace devices {

    struct DeviceSwitchEvent {
        const uint64_t deviceId;
        const uint8_t switchNumber;
        const tfw::hal::buttons::ButtonState state;
    };

    struct DeviceRotaryEncoderEvent {
        const uint64_t deviceId;
        const uint8_t encoderNumber;
        const tfw::hal::encoders::RotaryEncoderDirection direction;
    };

    class Device {
    public:
        virtual ~Device() = default;

        virtual void setup() = 0;
        virtual void loop() = 0;

        virtual const DeviceConfiguration& getConfiguration() const = 0;
        virtual const std::vector<std::shared_ptr<DeviceCapability>>& getCapabilities() const = 0;

        virtual tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) = 0;
        virtual tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) = 0;
        virtual tfw::utils::void_result rename(const std::string_view& deviceName) = 0;

        tfw::utils::observables::Observable<DeviceSwitchEvent>& onSwitchEvent() { return deviceSwitchEventSubject; }

        tfw::utils::observables::Observable<DeviceRotaryEncoderEvent>& onRotaryEncoderEvent() {
            return deviceRotaryEncoderEventSubject;
        }

    protected:
        tfw::utils::observables::Subject<DeviceSwitchEvent> deviceSwitchEventSubject;
        tfw::utils::observables::Subject<DeviceRotaryEncoderEvent> deviceRotaryEncoderEventSubject;
    };

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

}
