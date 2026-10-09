#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "DeviceCapabilities.h"

namespace mkp::devices::common {
    // Resolves the capabilities of a sub-device from its type, e.g. "smc1", which are unique across all parent
    // device types
    const std::vector<std::shared_ptr<DeviceCapability>>& subDeviceCapabilitiesFor(std::string_view deviceType);
}
