#pragma once

#include "utils/strings.h"

#include "MacroStorage.h"

namespace common::macros {
    template <class T>
    class MacroDataStorageSerializer {
    public:
        virtual ~MacroDataStorageSerializer() = default;

        virtual bool handles(MacroType type) = 0;

        virtual std::string_view serialize(
            const T& data,
            Arena& arena
        ) = 0;

        virtual std::shared_ptr<T> deserialize(
            uint16_t macroId,
            const std::span<const std::string_view>& parts,
            Arena& arena
        ) = 0;
    };


    extern std::vector<void*> macroDataSerializers;
}
