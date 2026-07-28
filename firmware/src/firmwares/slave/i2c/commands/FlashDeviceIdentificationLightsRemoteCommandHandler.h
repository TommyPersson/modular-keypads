#pragma once

#include "mkp/devices/common/LocalDevice.h"

#include <tfw/hal/i2c.h>

namespace firmwares::slave::i2c::commands {
    using namespace tfw::hal::i2c::commands;

    struct FlashDeviceIdentificationLightsParams {
        uint32_t durationMs;
    };

    inline RemoteCommandDescriptor<FlashDeviceIdentificationLightsParams> FlashDeviceIdentificationLights = {.id = 0x10};

    class FlashDeviceIdentificationLightsRemoteCommandHandler : public RemoteCommandHandler<FlashDeviceIdentificationLightsParams> {
    public:
        explicit FlashDeviceIdentificationLightsRemoteCommandHandler(mkp::devices::common::LocalDevice& device);

        ~FlashDeviceIdentificationLightsRemoteCommandHandler() override;

        tfw::utils::void_result execute(const FlashDeviceIdentificationLightsParams* params) override;

    private:
        mkp::devices::common::LocalDevice& device;
    };
}
