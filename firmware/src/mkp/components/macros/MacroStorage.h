#pragma once

#include <functional>
#include <optional>
#include <tfw/utils/allocations.h>

#include <tfw/utils/observables.h>
#include <tfw/utils/observables.h>

#include "Macro.h"

namespace common::macros {
    struct MacroSaved {
        uint16_t macroId;
    };

    struct MacroRemoved {
        uint16_t macroId;
    };

    class MacroStorage {
    public:
        void setup();

        error_t write(const Macro& macro);
        error_t remove(uint16_t id);

        void forEach(const std::function<void(const Macro&)>& callback);

        tfw::utils::observables::Observable<MacroSaved>& onMacroSaved() { return onMacroSavedSubject; };
        tfw::utils::observables::Observable<MacroRemoved>& onMacroRemoved() { return onMacroRemovedSubject; };

        uint64_t getNumStored();

    private:
        tfw::utils::observables::Subject<MacroSaved> onMacroSavedSubject;
        tfw::utils::observables::Subject<MacroRemoved> onMacroRemovedSubject;

        std::optional<uint64_t> numStored{};
    };
}
