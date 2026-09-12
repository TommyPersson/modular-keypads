#pragma once

#include "KeyBinding.h"

#include <functional>
#include <tfw/hal/fs.h>
#include <tfw/utils/observables.h>

namespace mkp::components::keybindings {
    struct KeyBindingSet {
    };

    struct KeyBindingCleared {
    };

    class KeyBindingStorage {
    public:
        explicit KeyBindingStorage(tfw::hal::fs::FileSystem& fileSystem);
        void setup();

        int write(const KeyBinding& keyBinding);
        int remove(const Trigger& trigger);
        int removeAll(const uint16_t& macroId);

        void forEach(const std::function<void(const KeyBinding&)>& callback);
        uint64_t count();

        tfw::utils::observables::Observable<KeyBindingSet>& onKeyBindingSet() { return onKeyBindingSetSubject; }
        tfw::utils::observables::Observable<KeyBindingCleared>& onKeyBindingCleared() { return onKeyBindingClearedSubject; }

    private:
        tfw::utils::observables::Subject<KeyBindingSet> onKeyBindingSetSubject;
        tfw::utils::observables::Subject<KeyBindingCleared> onKeyBindingClearedSubject;
        tfw::hal::fs::FileSystem& fileSystem;
    };
}
