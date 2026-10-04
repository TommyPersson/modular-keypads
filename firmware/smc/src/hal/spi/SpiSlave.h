#pragma once

#include <stdint.h>

#include "hal/base/Component.h"

namespace smc::hal::spi {
    namespace commands {
        inline constexpr uint8_t READ_DEVICE_ID = 0x01;
        inline constexpr uint8_t READ_DEVICE_TYPE = 0x02;
        inline constexpr uint8_t READ_EVENTS = 0x03;
        inline constexpr uint8_t ACK_EVENT = 0x04;
    }

    namespace ackStatus {
        inline constexpr uint8_t NOTHING_PENDING = 0x00;
        inline constexpr uint8_t REMOVED = 0x01;
    }

    /**
     * SPI0 slave on the alternate pins (SCK = PC0, MISO = PC1, MOSI = PC2, SS = PC3), mode 0, MSB first.
     *
     * A transaction (SS low to SS high) is one command byte from the master, followed by the
     * response bytes the master reads, followed by a CRC-8 byte (see utils/crc8.h) calculated over
     * the command byte as received and the response bytes. Unknown commands respond with only the
     * CRC byte. Reading beyond the CRC returns FILLER_BYTE. MISO is only driven while SS is low.
     *
     *   READ_DEVICE_ID: [id0, id1, id2, id3, crc]
     *   READ_EVENTS:    [type, data, crc] for the oldest event, or [NONE, 0, crc] when there are none.
     *                   The event is not removed, so reading again returns the same event.
     *   ACK_EVENT:      [status, crc]. Removes the event returned by the last READ_EVENTS (REMOVED),
     *                   or does nothing if it was already removed (NOTHING_PENDING), so it can be retried.
     *
     * ~INT (PC4, push-pull) is low while events are queued, and goes high when ACK_EVENT removes the
     * last one. The master can read and acknowledge events until INT goes high (or READ_EVENTS returns NONE).
     *
     * Timing: each byte is loaded by the interrupt that runs after the previous byte was
     * transferred, so the master must pause after every byte (including the command byte) for
     * the interrupt to complete. Code that disables interrupts (e.g. tinyNeoPixel::show())
     * delays this, and can corrupt a concurrent transaction, which the master detects through the CRC.
     */
    class SpiSlave : public Component {
    public:
        static constexpr uint8_t FILLER_BYTE = 0x00;

        SpiSlave() = default;
        ~SpiSlave() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        void setDeviceType(uint8_t deviceType);

        void enqueueEvent(const events::Event& event);

    private:
        uint8_t deviceType = 0;
        // Dequeued from the SPI interrupt; only modify with interrupts disabled
        events::EventQueue eventQueue;
    };
}
