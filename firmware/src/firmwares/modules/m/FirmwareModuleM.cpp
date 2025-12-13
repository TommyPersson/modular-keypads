#include "FirmwareModuleM.h"

#include <firmwares/common/DeviceProxy.h>
#include <firmwares/common/DeviceScanner.h>

#include "DeviceRuntimeM.h"
#include "LocalRegisterRefresherM.h"

FirmwareModuleM::FirmwareModuleM(ServiceLocator& serviceLocator) :
    Firmware(serviceLocator) {

    indicatorLeds = IndicatorLedManager::NeoPixel(5, 7);

    registerRefresher = std::make_unique<LocalRegisterRefresherM>(*registers);
    runtime = std::make_unique<DeviceRuntimeM>(*registers, *indicatorLeds, notifier, logger, DeviceMode::Local);
}

FirmwareModuleM::~FirmwareModuleM() = default;

void FirmwareModuleM::setup() {
    Firmware::setup();

    registerRefresher->begin();
    runtime->begin();

    i2c.begin(2, 1);

    logger.info("FirmwareModuleM:started");

    delay(100);

    I2cClient i2cClient(i2c);
    DeviceScanner scanner(i2cClient);
    auto scanResult = scanner.scan();

    for (auto& device : scanResult) {
        logger.info("Found device at %i: %s", device->getConfiguration().address, device->getConfiguration().id.c_str());
        logger.info("Device name: %s", device->getConfiguration().name.c_str());
        logger.info("Device type: %c", device->getConfiguration().type);
    }
}

void FirmwareModuleM::loop() {
    Firmware::loop();

    registerRefresher->loop();
    runtime->loop();
}
