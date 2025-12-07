#include "FirmwareModuleM.h"

#include <firmwares/common/i2c/EndpointStructs.h>

#include "DeviceRuntimeM.h"
#include "LocalRegisterRefresherM.h"

FirmwareModuleM::FirmwareModuleM(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger,
    TwoWire& i2c
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger, i2c) {

    indicatorLeds = IndicatorLedManager::NeoPixel(5, 7);

    registerRefresher = std::make_unique<LocalRegisterRefresherM>(*registers);
    runtime = std::make_unique<DeviceRuntimeM>(*registers, *indicatorLeds, notifier, logger, DeviceMode::Local);
}

FirmwareModuleM::~FirmwareModuleM() {
}

void FirmwareModuleM::setup() {
    Firmware::setup();

    registerRefresher->begin();
    runtime->begin();

    i2c.begin(2, 1);

    logger.info("FirmwareModuleM:started");

    // TODO clean up
    delay(100);
    for (int address = 10; address < 12; address++) {
        logger.info(" probing address %i", address);
        i2c.beginTransmission(address);
        uint8_t message[] = {1, 1};
        i2c.write(message, 2);
        auto rv = i2c.endTransmission();
        if (rv == 0) {
            logger.info("slave found at %i", address);
            i2c.requestFrom(address, sizeof(i2c::structs::DeviceInformation));
            uint8_t buffer[32];

            int i = 0;
            while (i2c.available()) {
                buffer[i] = i2c.read();
                i++;
            }

            const i2c::structs::DeviceInformation* deviceInformation = reinterpret_cast<i2c::structs::DeviceInformation
                *>(buffer);
            logger.info("deviceId = %.*s. type = %c", 16, deviceInformation->deviceId, deviceInformation->deviceType);
        }
    }
}

void FirmwareModuleM::loop() {
    Firmware::loop();

    registerRefresher->loop();
    runtime->loop();
}