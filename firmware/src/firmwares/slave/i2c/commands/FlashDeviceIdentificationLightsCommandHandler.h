#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include "utils/i2c/Commands.h"

namespace firmwares::slave::i2c::commands {
    using namespace utils::i2c::commands;

    struct FlashDeviceIdentificationLightsParams {
        uint32_t durationMs;
    };

    inline CommandDescriptor<FlashDeviceIdentificationLightsParams> FlashDeviceIdentificationLights = {.id = 0x10};

    class FlashDeviceIdentificationLightsCommandHandler : public CommandHandler<FlashDeviceIdentificationLightsParams> {
    public:
        explicit FlashDeviceIdentificationLightsCommandHandler(devices::DeviceModule& device);

        ~FlashDeviceIdentificationLightsCommandHandler() override;

        utils::void_result execute(const FlashDeviceIdentificationLightsParams* params) override;

    private:
        devices::DeviceModule& device;
    };
}
