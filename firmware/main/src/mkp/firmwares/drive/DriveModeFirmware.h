#pragma once

#include <memory>

#include <tfw/hal/time/Clock.h>

#include "BootButton.h"
#include "mkp/components/firmware/FirmwareDrive.h"
#include "mkp/components/firmware/FirmwareUpdater.h"

namespace mkp::firmwares::drive {
    /**
     * The firmware of the drive mode, which the master is restarted into with the boot button, where it only presents
     * the firmware drive. The keys, the config tool and everything else is left out, so that the device clearly is in
     * another mode rather than partly working, and as the drive doesn't fit beside all the other USB devices.
     *
     * The boot button, or updating the firmware, restarts the device in the normal mode again, as does power loss.
     */
    class DriveModeFirmware {
    public:
        explicit DriveModeFirmware(const tfw::hal::time::Clock& clock);
        ~DriveModeFirmware();

        void setup();
        void loop();

    private:
        std::unique_ptr<components::firmware::FirmwareUpdater> firmwareUpdater;
        std::unique_ptr<components::firmware::FirmwareDrive> firmwareDrive;
        std::unique_ptr<BootButton> bootButton;
    };
}
