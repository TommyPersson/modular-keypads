#include "FirmwareUpdater.h"

#include <cinttypes>

#include <Arduino.h>
#include <Update.h>

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("FirmwareUpdater");

    constexpr uint64_t IDLE_TIMEOUT_US = 10'000'000;

    // Long enough for the response to the finishing command to reach the host
    constexpr uint64_t RESTART_DELAY_US = 500'000;
}

using namespace mkp::components::firmware;

FirmwareUpdater::FirmwareUpdater(const tfw::hal::time::Clock& clock) {
    idleTimeoutCountDown = clock.createCountDown(IDLE_TIMEOUT_US, [this] {
        logger->warn("Firmware update timed out after %" PRIu32 " of %" PRIu32 " bytes", writtenSize, expectedSize);
        abort();
    });

    restartCountDown = clock.createCountDown(RESTART_DELAY_US, [] {
        logger->info("Restarting into the updated firmware");
        ESP.restart();
    });
}

FirmwareUpdater::~FirmwareUpdater() = default;

tfw::utils::void_result FirmwareUpdater::begin(const uint32_t size) {
    if (restartCountDown->isRunning()) {
        return tfw::utils::void_result::error("firmware.update.restart.pending");
    }

    abort();

    if (size == 0) {
        return tfw::utils::void_result::error("firmware.update.empty");
    }

    if (!Update.begin(size, U_FLASH)) {
        logger->error("Unable to begin firmware update: %s", Update.errorString());
        return tfw::utils::void_result::error("firmware.update.begin.failed");
    }

    logger->info("Firmware update started, %" PRIu32 " bytes", size);

    inProgress = true;
    expectedSize = size;
    writtenSize = 0;
    idleTimeoutCountDown->start();

    return tfw::utils::void_result::success();
}

tfw::utils::void_result FirmwareUpdater::write(const uint32_t offset, const std::span<const uint8_t> data) {
    if (!inProgress) {
        return tfw::utils::void_result::error("firmware.update.not.started");
    }

    if (offset != writtenSize) {
        return tfw::utils::void_result::error("firmware.update.unexpected.offset");
    }

    if (writtenSize + data.size() > expectedSize) {
        return tfw::utils::void_result::error("firmware.update.too.large");
    }

    // Update takes a non-const buffer, but only reads it
    const auto written = Update.write(const_cast<uint8_t*>(data.data()), data.size());
    if (written != data.size()) {
        logger->error("Unable to write firmware at %" PRIu32 ": %s", offset, Update.errorString());
        abort();
        return tfw::utils::void_result::error("firmware.update.write.failed");
    }

    writtenSize += written;
    idleTimeoutCountDown->start();

    return tfw::utils::void_result::success();
}

tfw::utils::void_result FirmwareUpdater::finish() {
    if (!inProgress) {
        return tfw::utils::void_result::error("firmware.update.not.started");
    }

    if (writtenSize != expectedSize) {
        return tfw::utils::void_result::error("firmware.update.incomplete");
    }

    inProgress = false;
    idleTimeoutCountDown->stop();

    if (!Update.end()) {
        logger->error("Unable to finish firmware update: %s", Update.errorString());
        return tfw::utils::void_result::error("firmware.update.verification.failed");
    }

    logger->info("Firmware update written, %" PRIu32 " bytes", writtenSize);
    restartCountDown->start();

    return tfw::utils::void_result::success();
}

void FirmwareUpdater::abort() {
    if (inProgress) {
        logger->info("Firmware update aborted");
        Update.abort();
    }

    inProgress = false;
    idleTimeoutCountDown->stop();
}

void FirmwareUpdater::loop() {
    idleTimeoutCountDown->update();
    restartCountDown->update();
}
