#pragma once

namespace mkp::firmwares::base {
    /// The date the firmware was built and the git commit it was built from, e.g. "2026-10-24-2444b54", with "-dirty"
    /// when built with uncommitted changes to the firmware, see scripts/firmware_version.py
    const char* firmwareVersion();
}
