#include "ClearKeyBindingCommandHandler.h"

#include <utils/strings.h>

using namespace common::keybindings;

ClearKeyBindingCommandHandler::ClearKeyBindingCommandHandler(
    KeyBindingStorage& keyBindingStorage
) : CommandHandler("clear.key.binding"),
    keyBindingStorage(keyBindingStorage) {}

ClearKeyBindingCommandHandler::~ClearKeyBindingCommandHandler() = default;

void ClearKeyBindingCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    const auto type = utils::strings::atol(args[0], 16);
    const auto deviceId = utils::strings::atou64(args[1], 16);

    if (type == PUSH_BUTTON) {
        const auto number = utils::strings::atol(args[2], 16);

        const PushButtonTrigger trigger(deviceId, number);

        keyBindingStorage.remove(trigger);
    } else if (type == ROTARY_ENCODER) {
        const auto number = utils::strings::atol(args[2], 16);
        const auto direction = static_cast<RotationalEncoderDirection>(utils::strings::atol(args[3], 16));

        const RotaryEncoderTrigger trigger(deviceId, number, direction);

        keyBindingStorage.remove(trigger);
    }
}
