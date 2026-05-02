#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include <tfw/hal/i2c.h>

namespace firmwares::slave::i2c::commands {
    using namespace tfw::utils::i2c::commands;

    struct FlashDeviceIdentificationLightsParams {
        uint32_t durationMs;
    };

    inline RemoteCommandDescriptor<FlashDeviceIdentificationLightsParams> FlashDeviceIdentificationLights = {.id = 0x10};

    class FlashDeviceIdentificationLightsRemoteCommandHandler : public RemoteCommandHandler<FlashDeviceIdentificationLightsParams> {
    public:
        explicit FlashDeviceIdentificationLightsRemoteCommandHandler(devices::DeviceModule& device);

        ~FlashDeviceIdentificationLightsRemoteCommandHandler() override;

        tfw::utils::void_result execute(const FlashDeviceIdentificationLightsParams* params) override;

    private:
        devices::DeviceModule& device;
    };
}
