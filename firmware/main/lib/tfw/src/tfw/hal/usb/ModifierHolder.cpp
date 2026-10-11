#include "ModifierHolder.h"

using namespace tfw::hal::usb;

ModifierChange ModifierHolder::hold(const uint8_t modifiers, const uint64_t nowUs) {
    const ModifierChange change{
        .press = static_cast<uint8_t>(modifiers & ~held),
        .release = static_cast<uint8_t>(held & ~modifiers),
    };

    held = modifiers;
    lastHoldUs = nowUs;

    return change;
}

ModifierChange ModifierHolder::releaseAll() {
    const ModifierChange change{.release = held};
    held = 0;
    return change;
}

ModifierChange ModifierHolder::update(const uint64_t nowUs) {
    if (held != 0 && nowUs - lastHoldUs >= holdTimeUs) {
        return releaseAll();
    }

    return {};
}
