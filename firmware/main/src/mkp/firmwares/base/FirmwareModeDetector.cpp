#include "FirmwareModeDetector.h"

#include <esp_attr.h>
#include <esp_system.h>

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("FirmwareModeDetector");

    // How long a host gets to enumerate the device after the USB stack has started
    constexpr uint32_t ENUMERATION_TIMEOUT_MS = 3'000;

    constexpr uint32_t ENUMERATION_POLL_INTERVAL_MS = 10;

    // Arbitrary, as memory that isn't initialized holds random values after a power-on
    constexpr uint32_t RESTART_AS_MASTER_MAGIC = 0x4d415354;

    // Kept over restarts, but not over power losses
    RTC_NOINIT_ATTR uint32_t restartAsMasterRequest;

    bool consumeRestartAsMasterRequest() {
        const auto requested = restartAsMasterRequest == RESTART_AS_MASTER_MAGIC;
        restartAsMasterRequest = 0;
        return requested;
    }
}

using namespace mkp::firmwares::base;

FirmwareModeDetector::FirmwareModeDetector(tfw::hal::usb::Connection& usbConnection, tfw::hal::time::Clock& clock) :
    usbConnection(usbConnection),
    clock(clock) {
}

FirmwareMode FirmwareModeDetector::detectFirmwareMode() const {
    const auto restartedAsMaster = consumeRestartAsMasterRequest();

    if (!usbConnection.isStarted()) {
        logger->info("USB not started, starting as a slave");
        return FirmwareMode::Slave;
    }

    // The host enumerated the device just before the restart
    if (restartedAsMaster) {
        logger->info("Restarted as the master");
        return FirmwareMode::Master;
    }

    for (uint32_t waitedMs = 0; waitedMs < ENUMERATION_TIMEOUT_MS; waitedMs += ENUMERATION_POLL_INTERVAL_MS) {
        if (usbConnection.isConnected()) {
            logger->info("Enumerated by a USB host, starting as the master");
            return FirmwareMode::Master;
        }

        clock.delayMs(ENUMERATION_POLL_INTERVAL_MS);
    }

    logger->info("Not enumerated by a USB host, starting as a slave");
    return FirmwareMode::Slave;
}

void FirmwareModeDetector::restartAsMaster() {
    restartAsMasterRequest = RESTART_AS_MASTER_MAGIC;
    esp_restart();
}
