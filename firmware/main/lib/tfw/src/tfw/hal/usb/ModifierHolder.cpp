#include "ModifierHolder.h"

using namespace tfw::hal::usb;

namespace {
    // Modifier key codes 0xe0 to 0xe7 are in the same order as the bits of the modifier flags
    constexpr uint8_t FIRST_MODIFIER_KEY_CODE = 0xe0;
}

void ModifierHolder::hold(const uint8_t modifiers, const uint64_t nowUs, const std::optional<uint64_t> holdTimeUs) {
    releaseModifiers(held & ~modifiers);
    pressModifiers(modifiers & ~held);

    held = modifiers;
    lastHoldUs = nowUs;
    this->holdTimeUs = holdTimeUs.value_or(defaultHoldTimeUs);
}

void ModifierHolder::releaseAll() {
    releaseModifiers(held);
    held = 0;
}

void ModifierHolder::update(const uint64_t nowUs) {
    if (held != 0 && nowUs - lastHoldUs >= holdTimeUs) {
        releaseAll();
    }
}

void ModifierHolder::pressModifiers(const uint8_t modifiers) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (modifiers & (1 << bit)) {
            hostComputer.pressKey(FIRST_MODIFIER_KEY_CODE + bit);
        }
    }
}

void ModifierHolder::releaseModifiers(const uint8_t modifiers) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (modifiers & (1 << bit)) {
            hostComputer.releaseKey(FIRST_MODIFIER_KEY_CODE + bit);
        }
    }
}
