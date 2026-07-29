#pragma once

#include <tfw/utils/commands.h>
#include "firmwares/common/keybindings/KeyBindingStorage.h"

namespace mkp::firmwares::master::commands {
    class SetKeyBinding : public tfw::utils::commands::CommandHandler {
    public:
        explicit SetKeyBinding(common::keybindings::KeyBindingStorage& keyBindingStorage);
        ~SetKeyBinding() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        common::keybindings::KeyBindingStorage& keyBindingStorage;
    };
}