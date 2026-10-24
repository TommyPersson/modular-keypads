#include "ActionSender.h"

#include <algorithm>
#include <optional>

using namespace tfw::hal::usb;

namespace {
    // Limit of the signed byte movement values in the HID mouse report
    constexpr int32_t MAX_MOUSE_REPORT_MOVEMENT = 127;

    int8_t takeMouseReportMovement(int32_t& remaining) {
        const auto part = std::clamp(remaining, -MAX_MOUSE_REPORT_MOVEMENT, MAX_MOUSE_REPORT_MOVEMENT);
        remaining -= part;
        return static_cast<int8_t>(part);
    }
}

ActionSender::ActionSender(HostComputer& hostComputer, const time::Clock& clock)
    : hostComputer(hostComputer),
      clock(clock),
      modifierHolder(hostComputer) {
}

void ActionSender::send(Action& action) {
    if (const auto shortcutAction = dynamic_cast<ShortcutAction*>(&action)) {
        sendShortcut(*shortcutAction);
    } else if (const auto consumerControlAction = dynamic_cast<ConsumerControlAction*>(&action)) {
        sendConsumerControl(*consumerControlAction);
    } else if (const auto systemControlAction = dynamic_cast<SystemControlAction*>(&action)) {
        sendSystemControl(*systemControlAction);
    } else if (const auto typeAction = dynamic_cast<TypeAction*>(&action)) {
        sendType(*typeAction);
    } else if (const auto mouseAction = dynamic_cast<MouseAction*>(&action)) {
        sendMouse(*mouseAction);
    }
}

void ActionSender::update() {
    const auto now = clock.micros();

    if (remainingMouseX != 0 || remainingMouseY != 0) {
        // Holding again keeps the modifiers held until the hold time has passed after the last part, and presses them
        // again if other actions released them in between
        modifierHolder.hold(mouseModifiers, now);
        sendMouseMovementPart(0, 0);
    }

    // Releasing modifiers in the middle of a movement would tap them between its parts, with a zero hold time
    if (remainingMouseX == 0 && remainingMouseY == 0) {
        modifierHolder.update(now);
    }

    // Typing waits while modifiers are held, as they would turn the characters into shortcuts. Without a delay
    // between characters, repeated characters can be missed.
    const auto enoughTimePassSinceLastWrite = now - lastKeyboardWriteTime >= typingDelayUs;
    const auto canWrite = enoughTimePassSinceLastWrite && !modifierHolder.isHeld();
    if (canWrite) {
        const auto nextChar = characterOutputQueue.dequeue();
        if (nextChar != nullptr) {
            hostComputer.typeCharacter(*nextChar);

            // Only updated on writes, so that the first character of a text isn't delayed
            lastKeyboardWriteTime = now;
        }
    }
}

bool ActionSender::isBusy() const {
    return characterOutputQueue.size() > 0 || remainingMouseX != 0 || remainingMouseY != 0;
}

void ActionSender::setTypingDelayMs(const uint32_t delayMs) {
    typingDelayUs = static_cast<uint64_t>(delayMs) * 1'000;
}

void ActionSender::setModifierHoldMs(const uint32_t holdMs) {
    modifierHolder.setDefaultHoldTimeUs(static_cast<uint64_t>(holdMs) * 1'000);
}

void ActionSender::sendShortcut(const ShortcutAction& action) {
    const auto holdTimeUs = action.holdTimeMs > 0
        ? std::optional(static_cast<uint64_t>(action.holdTimeMs) * 1'000)
        : std::nullopt;

    modifierHolder.hold(action.modifiers, clock.micros(), holdTimeUs);

    if (action.keyCode != 0) {
        hostComputer.pressKey(action.keyCode);
        hostComputer.releaseKey(action.keyCode);
    }

    if (!action.holdModifiers) {
        modifierHolder.releaseAll();
    }
}

void ActionSender::sendConsumerControl(const ConsumerControlAction& action) {
    // Modifiers held for other actions have no use here
    modifierHolder.releaseAll();

    hostComputer.pressConsumerControl(action.usageId);
    hostComputer.releaseConsumerControl();
}

void ActionSender::sendSystemControl(const SystemControlAction& action) {
    // Modifiers held for other actions have no use here
    modifierHolder.releaseAll();

    hostComputer.pressSystemControl(action.code);
    hostComputer.releaseSystemControl();
}

void ActionSender::sendType(const TypeAction& action) {
    // Modifiers held for other actions would turn the characters into shortcuts
    modifierHolder.releaseAll();

    for (const auto& character : action.text) {
        characterOutputQueue.enqueue(character);
    }
}

void ActionSender::sendMouse(const MouseAction& action) {
    modifierHolder.hold(action.modifiers, clock.micros());

    if (action.buttons != 0) {
        hostComputer.clickMouse(action.buttons);
    }

    // Added to movement still being sent by earlier actions, so that it keeps its order. The first part is sent right
    // away, along with the scrolling, and the rest by update().
    remainingMouseX += action.x;
    remainingMouseY += action.y;
    mouseModifiers = action.modifiers;

    sendMouseMovementPart(action.wheel, action.pan);
}

void ActionSender::sendMouseMovementPart(const int8_t wheel, const int8_t pan) {
    const auto x = takeMouseReportMovement(remainingMouseX);
    const auto y = takeMouseReportMovement(remainingMouseY);

    if (x != 0 || y != 0 || wheel != 0 || pan != 0) {
        hostComputer.moveMouse(x, y, wheel, pan);
    }
}
