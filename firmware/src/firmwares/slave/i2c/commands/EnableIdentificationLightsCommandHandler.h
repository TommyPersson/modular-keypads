#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include "utils/i2c/Commands.h"

namespace firmwares::slave::i2c::commands {
    using namespace utils::i2c::commands;

    struct EnableIdentificationLightsParams {
        uint32_t durationMs;
    };

    inline CommandDescriptor<EnableIdentificationLightsParams> EnableIdentificationLights = {.id = 0x10};

    class EnableIdentificationLightsCommandHandler : public CommandHandler<EnableIdentificationLightsParams> {
    public:
        explicit EnableIdentificationLightsCommandHandler(devices::DeviceModule& device);

        ~EnableIdentificationLightsCommandHandler() override;

        utils::void_result execute(const EnableIdentificationLightsParams* params) override;

    private:
        devices::DeviceModule& device;
    };
}
