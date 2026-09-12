#pragma once

#include <functional>
#include <optional>
#include <tfw/utils/allocations.h>
#include <tfw/hal/fs.h>

#include <tfw/utils/observables.h>

#include "Macro.h"

namespace mkp::components::macros {
    struct MacroSaved {
        uint16_t macroId;
    };

    struct MacroRemoved {
        uint16_t macroId;
    };

    class MacroStorage {
    public:
        explicit MacroStorage(tfw::hal::fs::FileSystem& fileSystem);
        void setup();

        int write(const Macro& macro);
        int remove(uint16_t id);

        void forEach(const std::function<void(const Macro&)>& callback);
        uint64_t count();

        tfw::utils::observables::Observable<MacroSaved>& onMacroSaved() { return onMacroSavedSubject; };
        tfw::utils::observables::Observable<MacroRemoved>& onMacroRemoved() { return onMacroRemovedSubject; };

        uint64_t getNumStored();

    private:
        tfw::utils::observables::Subject<MacroSaved> onMacroSavedSubject;
        tfw::utils::observables::Subject<MacroRemoved> onMacroRemovedSubject;

        std::optional<uint64_t> numStored{};
        tfw::hal::fs::FileSystem& fileSystem;
    };
}
