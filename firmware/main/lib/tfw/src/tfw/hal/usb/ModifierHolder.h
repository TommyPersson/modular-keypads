#pragma once

#include <cstdint>
#include <optional>

#include "HostComputer.h"

namespace tfw::hal::usb {
    // Keeps keyboard modifiers held across consecutive actions that use them, such as mouse actions and shortcuts that
    // hold their modifiers, and releases them once no such action has arrived for the hold time. Repeated actions,
    // such as fast scrolling, then become a single modifier press, instead of one press per action. Repeated taps of a
    // modifier can trigger things of their own, such as the Sticky Keys prompt after pressing shift five times.
    //
    // Modifiers are given as bit flags in HID keyboard report order (bit 0 = left control, ...).
    class ModifierHolder {
    public:
        explicit ModifierHolder(HostComputer& hostComputer) : hostComputer(hostComputer) {
        }

        // Hold time for actions that don't have one of their own
        void setDefaultHoldTimeUs(const uint64_t holdTimeUs) { defaultHoldTimeUs = holdTimeUs; }

        // Holds exactly the given modifiers, for an action at the given time, pressing and releasing modifiers as
        // needed. They are released once the hold time of this action, or the default hold time, has passed without
        // another hold.
        void hold(uint8_t modifiers, uint64_t nowUs, std::optional<uint64_t> holdTimeUs = std::nullopt);

        // Releases all held modifiers, before actions that need the keyboard to themselves
        void releaseAll();

        // Releases the held modifiers once the hold time has passed since the last action that used them
        void update(uint64_t nowUs);

        uint8_t getHeld() const { return held; }

        bool isHeld() const { return held != 0; }

    private:
        void pressModifiers(uint8_t modifiers);
        void releaseModifiers(uint8_t modifiers);

        HostComputer& hostComputer;
        uint64_t defaultHoldTimeUs = 250'000;
        uint64_t holdTimeUs = 250'000;
        uint8_t held = 0;
        uint64_t lastHoldUs = 0;
    };
}
