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

        tfw::utils::observables::Observable<KeyBindingSet>& onKeyBindingSet() { return onKeyBindingSetSubject; }
        tfw::utils::observables::Observable<KeyBindingCleared>& onKeyBindingCleared() { return onKeyBindingClearedSubject; }

    private:
        tfw::utils::observables::Subject<KeyBindingSet> onKeyBindingSetSubject;
        tfw::utils::observables::Subject<KeyBindingCleared> onKeyBindingClearedSubject;
    };
}
