#pragma once

#include "mkp/devices/common/LocalDevice.h"

#include <tfw/hal/i2c.h>

namespace firmwares::slave::i2c::commands {
    using namespace tfw::hal::i2c::commands;

    struct RenameDeviceParams {
        char name[16];
    };

    inline RemoteCommandDescriptor<RenameDeviceParams> RenameDevice = {.id = 0x11};

    class RenameDeviceRemoteCommandHandler : public RemoteCommandHandler<RenameDeviceParams> {
    public:
        explicit RenameDeviceRemoteCommandHandler(mkp::devices::common::LocalDevice& device);

        ~RenameDeviceRemoteCommandHandler() override;

        tfw::utils::void_result execute(const RenameDeviceParams* params) override;

    private:
        mkp::devices::common::LocalDevice& device;
    };
}
