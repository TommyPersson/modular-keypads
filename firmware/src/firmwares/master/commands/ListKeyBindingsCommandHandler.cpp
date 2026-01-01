#include "ListKeyBindingsCommandHandler.h"

using namespace common::keybindings;

ListKeyBindingsCommandHandler::ListKeyBindingsCommandHandler(
    KeyBindingStorage& keyBindingStorage
) : CommandHandler("list.key.bindings"),
    keyBindingStorage(keyBindingStorage) {}

ListKeyBindingsCommandHandler::~ListKeyBindingsCommandHandler() = default;

utils::void_result ListKeyBindingsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    keyBindingStorage.forEach([&](const KeyBinding& keyBinding) {
        if (keyBinding.trigger->type == PUSH_BUTTON) {
            const auto trigger = dynamic_cast<PushButtonTrigger&>(*keyBinding.trigger);

            responseWriter.writeLineF(
                "0x%02x:%08llx:0x%02x:0x%04x",
                trigger.type,
                trigger.deviceId,
                trigger.number,
                keyBinding.macroId
            );
        } else if (keyBinding.trigger->type == ROTARY_ENCODER) {
            const auto trigger = dynamic_cast<RotaryEncoderTrigger&>(*keyBinding.trigger);

            responseWriter.writeLineF(
                "0x%02x:%08llx:0x%02x:0x%02x:0x%04x",
                trigger.type,
                trigger.deviceId,
                trigger.number,
                trigger.direction,
                keyBinding.macroId
            );
        }
    });

    return utils::void_result::success();
}
