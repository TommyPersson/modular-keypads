#pragma once

#include <cstdint>
#include "Event.h"

namespace tfw::hal::i2c {
    template <class TStruct>
    struct EndpointDescriptor {
        std::uint8_t id = 0;
        std::uint8_t length = sizeof(TStruct);
    };

    const std::uint8_t MAX_PACKET_SIZE = 30; // 32 is the underlying Wire buffer, but if we use it we often read ETB characters

#pragma pack(push, 1)
    /**
     * Read from temporary addresses to tell whether one or several devices answered. Reads from several devices are
     * the bitwise AND of their responses, which can only hold an ID and its complement when the IDs are equal.
     */
    struct Identity {
        std::uint64_t id = 0;
        std::uint64_t idComplement = 0;

        static Identity of(const std::uint64_t id) { return {.id = id, .idComplement = ~id}; }

        bool isValid() const { return id == ~idComplement; }
    };
#pragma pack(pop)

    namespace endpoints::builtin {
        inline EndpointDescriptor<Event> Events{.id = 0xee};
        inline EndpointDescriptor<Identity> Identity{.id = 0xef};
    }
};
