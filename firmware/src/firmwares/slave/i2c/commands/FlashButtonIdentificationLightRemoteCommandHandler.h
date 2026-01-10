#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include "utils/i2c/Commands.h"

namespace firmwares::slave::i2c::commands {
    using namespace utils::i2c::commands;

    struct FlashButtonIdentificationLightParams {
        uint8_t buttonNumber;
        uint32_t durationMs;
    };

    inline RemoteCommandDescriptor<FlashButtonIdentificationLightParams> FlashButtonIdentificationLight = {.id = 0x12};

    class FlashButtonIdentificationLightRemoteCommandHandler : public RemoteCommandHandler<FlashButtonIdentificationLightParams> {
    public:
        explicit FlashButtonIdentificationLightRemoteCommandHandler(devices::DeviceModule& device);

        ~FlashButtonIdentificationLightRemoteCommandHandler() override;

        utils::void_result execute(const FlashButtonIdentificationLightParams* params) override;

    private:
        devices::DeviceModule& device;
    };
}
