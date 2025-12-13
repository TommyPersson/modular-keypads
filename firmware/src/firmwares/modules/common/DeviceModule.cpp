#include "DeviceModule.h"

DeviceModule::DeviceModule(
    ServiceLocator& serviceLocator,
    std::unique_ptr<DeviceRuntime> deviceRuntime,
    std::unique_ptr<RegisterRefresher> registerRefresher
    ) {
}

DeviceModule::~DeviceModule() {
}

void DeviceModule::setup() {
}

void DeviceModule::loop() {
}

std::unique_ptr<DeviceModule> DeviceModule::create(char type, DeviceMode mode) {
    /*
    switch (type) {
    case 'm':
    case 'M':
        return std::make_unique<FirmwareModuleM>(serviceLocator);
    case 'a':
    case 'A':
        return std::make_unique<FirmwareModuleA>(serviceLocator);
    default:
        return std::make_unique<GenericFirmware>(serviceLocator);
    }*/

    return nullptr;
}
