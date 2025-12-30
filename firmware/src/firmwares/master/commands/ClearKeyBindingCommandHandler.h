#pragma once

#include "firmwares/common/commands/CommandHandler.h"
#include "firmwares/common/keybindings/KeyBindingStorage.h"

class ClearKeyBindingCommandHandler : public CommandHandler {
public:
    explicit ClearKeyBindingCommandHandler(common::keybindings::KeyBindingStorage& keyBindingStorage);
    ~ClearKeyBindingCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    common::keybindings::KeyBindingStorage& keyBindingStorage;
};
