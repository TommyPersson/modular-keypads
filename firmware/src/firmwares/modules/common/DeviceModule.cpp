#include "DeviceModule.h"

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceModule");
}


devices::DeviceModule::DeviceModule(
    const DeviceConfiguration& configuration,
    DeviceLocation deviceLocation,
    tfw::hal::i2c::Client& i2cClient
) : configuration(configuration),
    deviceLocation(deviceLocation),
    i2cClient(i2cClient) {
}

void devices::DeviceModule::setup() {
    this->getRuntime().onSwitchEvent().addObserver(this);
    this->getRuntime().onRotaryEncoderEvent().addObserver(this);
}

void devices::DeviceModule::loop() {
}

tfw::utils::void_result devices::DeviceModule::flashIdentificationLights(uint32_t durationMs) {
    if (deviceLocation == DeviceLocation::Local) {
        this->getRuntime().flashIdentificationLights(durationMs);
        return tfw::utils::void_result::success();
    }

    return tfw::utils::void_result::success();
}

tfw::utils::void_result
devices::DeviceModule::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    if (deviceLocation == DeviceLocation::Local) {
        this->getRuntime().flashButtonIdentificationLight(buttonNumber, durationMs);
        return tfw::utils::void_result::success();
    }

    return tfw::utils::void_result::success();
}

tfw::utils::void_result devices::DeviceModule::rename(const std::string_view& deviceName) {
    if (deviceLocation == DeviceLocation::Local) {
        return this->getRuntime().renameDevice(deviceName);
    }

    return tfw::utils::void_result::success();
}

void devices::DeviceModule::observe(const DeviceSwitchEvent& event) {
    if (deviceLocation == DeviceLocation::Local) { // TODO rewrite solution to split remote and local devices more clearly. dont rely on register refresing from remote devices.
        deviceSwitchEventSubject.notify(event);
    }
}

void devices::DeviceModule::observe(const DeviceRotaryEncoderEvent& event) {
    if (deviceLocation == DeviceLocation::Local) {
        deviceRotaryEncoderEventSubject.notify(event);
    }
}
