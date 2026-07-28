#include "DeviceModule.h"

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceModule");
}


devices::DeviceModule::DeviceModule(
    const DeviceConfiguration& configuration
) : configuration(configuration) {
}

void devices::DeviceModule::setup() {
    this->getRuntime().onSwitchEvent().addObserver(this);
    this->getRuntime().onRotaryEncoderEvent().addObserver(this);
}

void devices::DeviceModule::loop() {
}

tfw::utils::void_result devices::DeviceModule::flashIdentificationLights(uint32_t durationMs) {
    this->getRuntime().flashIdentificationLights(durationMs);
    return tfw::utils::void_result::success();
}

tfw::utils::void_result
devices::DeviceModule::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    this->getRuntime().flashButtonIdentificationLight(buttonNumber, durationMs);
    return tfw::utils::void_result::success();

    return tfw::utils::void_result::success();
}

tfw::utils::void_result devices::DeviceModule::rename(const std::string_view& deviceName) {
    return this->getRuntime().renameDevice(deviceName);
}

void devices::DeviceModule::observe(const DeviceSwitchEvent& event) {
    deviceSwitchEventSubject.notify(event);
}

void devices::DeviceModule::observe(const DeviceRotaryEncoderEvent& event) {
    deviceRotaryEncoderEventSubject.notify(event);
}
