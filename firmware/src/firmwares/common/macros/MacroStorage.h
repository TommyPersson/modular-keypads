#pragma once

#include <functional>
#include <optional>
#include <utils/allocations/ArenaUtils.h>

#include "utils/observables/Observable.h"
#include "utils/observables/Subject.h"

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

        utils::observables::Observable<MacroSaved>& onMacroSaved() { return onMacroSavedSubject; };
        utils::observables::Observable<MacroRemoved>& onMacroRemoved() { return onMacroRemovedSubject; };

        uint64_t getNumStored();

    private:
        utils::observables::Subject<MacroSaved> onMacroSavedSubject;
        utils::observables::Subject<MacroRemoved> onMacroRemovedSubject;

        std::optional<uint64_t> numStored{};
    };
}
