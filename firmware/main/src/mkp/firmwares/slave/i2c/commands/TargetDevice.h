#pragma once

#include "mkp/devices/common/LocalDevice.h"

namespace firmwares::slave::i2c::commands {
    // Resolves the device a command targets, which is either the local device or one of its connected sub-devices
    inline mkp::devices::common::Device* resolveTargetDevice(
        mkp::devices::common::LocalDevice& device,
        const uint64_t targetDeviceId
    ) {
        if (targetDeviceId == device.getConfiguration().id) {
            return &device;
        }

        if (targetDeviceId == 0) {
            return nullptr;
        }

        for (const auto subDevice : device.getSubDevices()) {
            if (subDevice->getConfiguration().id == targetDeviceId) {
                return subDevice;
            }
        }

        return nullptr;
    }
}
