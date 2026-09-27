#pragma once

#include <stdint.h>

#include "hal/base/Component.h"

namespace smc::hal::spi {
    namespace commands {
        inline constexpr uint8_t READ_DEVICE_ID = 0x01;
        inline constexpr uint8_t READ_EVENTS = 0x02;
    }

    /**
     * SPI0 slave on the alternate pins (SCK = PC0, MISO = PC1, MOSI = PC2, SS = PC3), mode 0, MSB first.
     *
     * A transaction (SS low to SS high) is one command byte from the master, followed by the
     * response bytes the master reads. Reading beyond the end of a response, or after an unknown
     * command, returns FILLER_BYTE. MISO is only driven while SS is low.
     *
     * Timing: the first response byte is loaded by the interrupt that receives the command byte,
     * so the master must pause briefly after the command byte before clocking the response.
     * The remaining bytes are buffered ahead and can be read back-to-back. Code that disables
     * interrupts (e.g. tinyNeoPixel::show()) delays this, and can corrupt a concurrent transaction.
     *
     * READ_EVENTS returns the next event queued with enqueueEvent(), as [type, data], or
     * [NONE, 0] when there are none.
     */
    class SpiSlave : public Component {
    public:
        static constexpr uint8_t FILLER_BYTE = 0x00;

        SpiSlave() = default;
        ~SpiSlave() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override {}

        void enqueueEvent(const events::Event& event);

    private:
        // Dequeued from the SPI interrupt; only modify with interrupts disabled
        events::EventQueue eventQueue;
    };
}
