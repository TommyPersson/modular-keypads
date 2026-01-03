#include "DeviceProxy.h"

DeviceProxy::DeviceProxy(const DeviceConfiguration& configuration, utils::i2c::Client& i2c) :
    configuration(configuration), i2c(i2c) {
}

DeviceProxy::~DeviceProxy() = default;

DeviceConfiguration& DeviceProxy::getConfiguration() {
    return configuration;
}

std::array<uint8_t, 32>& DeviceProxy::readRegisters() {
    // TODO refresh
    return registerData;
}
