#pragma once

#include "KeyBinding.h"

#include <functional>
#include <cerrno>

namespace common::keybindings {
    struct KeyBindingSet {
    };

    struct KeyBindingCleared {
    };

    class KeyBindingStorage {
    public:
        void setup();

        error_t write(const KeyBinding& keyBinding);
        error_t remove(const Trigger& trigger);
        error_t removeAll(const uint16_t& macroId);

        void forEach(const std::function<void(const KeyBinding&)>& callback);

        Observable<KeyBindingSet>& onKeyBindingSet() { return onKeyBindingSetSubject; }
        Observable<KeyBindingCleared>& onKeyBindingCleared() { return onKeyBindingClearedSubject; }

    private:
        Subject<KeyBindingSet> onKeyBindingSetSubject;
        Subject<KeyBindingCleared> onKeyBindingClearedSubject;
    };
}
