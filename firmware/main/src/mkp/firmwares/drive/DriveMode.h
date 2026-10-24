#pragma once

namespace mkp::firmwares::drive {
    /// Restarts into the drive mode, where the device only presents the firmware drive
    [[noreturn]] void restartInDriveMode();

    /// Whether the device was restarted into the drive mode. Only answered once, so that the next restart, e.g. into
    /// an updated firmware, is a normal one.
    bool consumeDriveModeRequest();
}
