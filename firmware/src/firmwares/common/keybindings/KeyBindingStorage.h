#pragma once

#include "KeyBinding.h"

#include <functional>
#include <cerrno>

namespace common::keybindings {
    class KeyBindingStorage {
    public:
        void setup();

        error_t write(const KeyBinding& keyBinding);
        error_t remove(const Trigger& trigger);

        void forEach(const std::function<void(const KeyBinding&)>& callback);
    };
}
