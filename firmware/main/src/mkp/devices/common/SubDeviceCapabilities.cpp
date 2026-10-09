#include "SubDeviceCapabilities.h"

using namespace mkp::devices::common;

namespace {
    // The SMC reports its own button events, so there are no registers. The LED indices are the SMC's own (see
    // the button definitions in the SMC firmwares), the SMC maps button numbers to them.
    const std::vector<std::shared_ptr<DeviceCapability>> smc1Capabilities = {
        std::make_shared<PushButtonCapability>(1, NoRegister, 0, 0),
        std::make_shared<PushButtonCapability>(2, NoRegister, 0, 1),
        std::make_shared<PushButtonCapability>(3, NoRegister, 0, 3),
        std::make_shared<PushButtonCapability>(4, NoRegister, 0, 2),
    };

    const std::vector<std::shared_ptr<DeviceCapability>> smc2Capabilities = {
        std::make_shared<PushButtonCapability>(1, NoRegister, 0, 0),
        std::make_shared<PushButtonCapability>(2, NoRegister, 0, 1),
        std::make_shared<PushButtonCapability>(3, NoRegister, 0, 2),
        std::make_shared<PushButtonCapability>(4, NoRegister, 0, 5),
        std::make_shared<PushButtonCapability>(5, NoRegister, 0, 4),
        std::make_shared<PushButtonCapability>(6, NoRegister, 0, 3),
    };

    const std::vector<std::shared_ptr<DeviceCapability>> noCapabilities = {};
}

const std::vector<std::shared_ptr<DeviceCapability>>& mkp::devices::common::subDeviceCapabilitiesFor(
    const std::string_view deviceType
) {
    if (deviceType == "smc1") {
        return smc1Capabilities;
    }

    if (deviceType == "smc2") {
        return smc2Capabilities;
    }

    return noCapabilities;
}
