#pragma once

#include <firmwares/modules/common/DeviceModule.h>

#include <tfw/hal/i2c.h>

namespace firmwares::slave::i2c::commands {
    using namespace tfw::utils::i2c::commands;

    struct RenameDeviceParams {
        char name[16];
    };

    inline RemoteCommandDescriptor<RenameDeviceParams> RenameDevice = {.id = 0x11};

    class RenameDeviceRemoteCommandHandler : public RemoteCommandHandler<RenameDeviceParams> {
    public:
        explicit RenameDeviceRemoteCommandHandler(devices::DeviceModule& device);

        ~RenameDeviceRemoteCommandHandler() override;

        tfw::utils::void_result execute(const RenameDeviceParams* params) override;

    private:
        devices::DeviceModule& device;
    };
}
