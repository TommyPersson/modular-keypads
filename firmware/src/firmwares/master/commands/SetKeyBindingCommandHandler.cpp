#include "SetKeyBindingCommandHandler.h"

#include <utils/strings.h>

using namespace common::keybindings;

SetKeyBindingCommandHandler::SetKeyBindingCommandHandler(
    KeyBindingStorage& keyBindingStorage
) : CommandHandler("set.key.binding"),
    keyBindingStorage(keyBindingStorage) {}

SetKeyBindingCommandHandler::~SetKeyBindingCommandHandler() = default;

utils::void_result SetKeyBindingCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    const auto type = utils::strings::atol(args[0], 16);
    const auto deviceId = utils::strings::atou64(args[1], 16);

    if (type == PUSH_BUTTON) {
        const auto number = utils::strings::atol(args[2], 16);
        const auto macroId = static_cast<uint16_t>(utils::strings::atol(args[3], 16));

        const auto keyBinding = KeyBinding{
            .trigger = std::make_shared<PushButtonTrigger>(deviceId, number),
            .macroId = macroId,
        };

        keyBindingStorage.write(keyBinding);
    } else if (type == ROTARY_ENCODER) {
        const auto number = utils::strings::atol(args[2], 16);
        const auto direction = static_cast<RotationalEncoderDirection>(utils::strings::atol(args[3], 16));
        const auto macroId = static_cast<uint16_t>(utils::strings::atol(args[4], 16));

        const auto keyBinding = KeyBinding{
            .trigger = std::make_shared<RotaryEncoderTrigger>(deviceId, number, direction),
            .macroId = macroId,
        };

        keyBindingStorage.write(keyBinding);
    } else {
        return utils::void_result::error("unknown.key.binding.type");
    }

    return utils::void_result::success();
}
