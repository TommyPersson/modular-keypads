#include "DriveMode.h"

#include <cstdint>

#include <esp_attr.h>
#include <esp_system.h>

namespace {
    constexpr uint32_t DRIVE_MODE_REQUESTED = 0x44524956;

    // Kept through restarts, as RTC memory isn't initialized on startup. Power loss leaves it with random content.
    RTC_NOINIT_ATTR uint32_t driveModeRequest;
}

void mkp::firmwares::drive::restartInDriveMode() {
    driveModeRequest = DRIVE_MODE_REQUESTED;
    esp_restart();
}

bool mkp::firmwares::drive::consumeDriveModeRequest() {
    // Only restarts are trusted, as the random content after power loss could happen to be the request
    const auto isRequested = esp_reset_reason() == ESP_RST_SW && driveModeRequest == DRIVE_MODE_REQUESTED;
    driveModeRequest = 0;

    return isRequested;
}
