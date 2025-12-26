#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "Macro.h"

namespace common::macros {

    class MacroStorage {
    public:
        void setup();

        error_t write(const Macro& macro);

        void forEach(const std::function<void(const Macro&)>& callback);

    private:
        std::vector<std::shared_ptr<Macro>> cache;
    };
}
