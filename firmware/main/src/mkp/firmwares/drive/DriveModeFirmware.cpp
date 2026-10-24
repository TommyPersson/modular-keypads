#include "DriveModeFirmware.h"

#include <Arduino.h>
#include <USB.h>

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("DriveModeFirmware");
}

using namespace mkp::firmwares::drive;

DriveModeFirmware::DriveModeFirmware(const tfw::hal::time::Clock& clock) {
    firmwareUpdater = std::make_unique<components::firmware::FirmwareUpdater>(clock);

    // Created before USB is started by the setup, as the drive is part of the USB device
    firmwareDrive = std::make_unique<components::firmware::FirmwareDrive>(*firmwareUpdater, clock);

    bootButton = std::make_unique<BootButton>(clock, [] {
        logger->info("Boot button clicked, restarting in the normal mode");
        ESP.restart();
    });
}

DriveModeFirmware::~DriveModeFirmware() = default;

void DriveModeFirmware::setup() {
    firmwareDrive->setup();
    bootButton->setup();

    USB.productName("tommy-product");
    USB.manufacturerName("tommy-manufacturer");
    USB.begin();

    logger->info("Started in the drive mode");
}

void DriveModeFirmware::loop() {
    firmwareUpdater->loop();
    firmwareDrive->loop();
    bootButton->loop();
}
