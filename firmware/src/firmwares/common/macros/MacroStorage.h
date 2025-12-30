#pragma once

#include <cerrno>
#include <functional>

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

        Observable<MacroSaved>& onMacroSaved() { return onMacroSavedSubject; };
        Observable<MacroRemoved>& onMacroRemoved() { return onMacroRemovedSubject; };

    private:
        Subject<MacroSaved> onMacroSavedSubject;
        Subject<MacroRemoved> onMacroRemovedSubject;
    };
}
