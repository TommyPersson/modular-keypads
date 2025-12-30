#pragma once

#include <cerrno>
#include <functional>

#include "Macro.h"

namespace common::macros {

    class MacroStorage {
    public:
        void setup();

        error_t write(const Macro& macro);
        error_t remove(uint16_t id);

        void forEach(const std::function<void(const Macro&)>& callback);
    };
}
