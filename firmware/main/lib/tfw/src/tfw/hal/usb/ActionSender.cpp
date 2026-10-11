#include "ActionSender.h"

#include <optional>

using namespace tfw::hal::usb;

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

    modifierHolder.update(now);

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
    return characterOutputQueue.size() > 0;
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

    if (action.x != 0 || action.y != 0 || action.wheel != 0 || action.pan != 0) {
        hostComputer.moveMouse(action.x, action.y, action.wheel, action.pan);
    }
}
