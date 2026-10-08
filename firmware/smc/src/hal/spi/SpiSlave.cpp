#include "SpiSlave.h"

#include <Arduino.h>
#include <string.h>
#include <util/atomic.h>

#include "utils/crc8.h"

using namespace smc::hal::spi;

// TODO verify

namespace {
    // Alternate SPI0 pins, all on PORTC
    constexpr uint8_t SCK_bm = PIN0_bm;
    constexpr uint8_t MISO_bm = PIN1_bm;
    constexpr uint8_t MOSI_bm = PIN2_bm;
    constexpr uint8_t SS_bm = PIN3_bm;

    // Push-pull, low while events are queued
    constexpr uint8_t INT_bm = PIN4_bm;

    uint8_t deviceId[smc::hal::identity::DEVICE_ID_LENGTH] = {};

    struct Response {
        const uint8_t* data;
        uint8_t length;
    };

    smc::events::EventQueue* eventQueue = nullptr;

    uint8_t deviceType = 0;

    // Whether the oldest queued event has been returned by READ_EVENTS and awaits an ACK_EVENT.
    // Modified from the ISRs, and from enqueueEvent() with interrupts disabled.
    bool hasReadEvent = false;

    // Transaction state, only accessed from the ISRs below
    bool awaitingCommand = true;
    Response response = {nullptr, 0};
    uint8_t responseIndex = 0;
    uint8_t responseCrc = 0;
    uint8_t eventResponse[2] = {};
    uint8_t ackResponse[1] = {};

    // TODO Temporary diagnostics, printed from update()
    volatile uint16_t ssFallingEdges = 0;
    volatile uint16_t ssRisingEdges = 0;
    volatile uint16_t spiInterrupts = 0;
    volatile uint16_t commandsReceived = 0;
    volatile uint16_t writeCollisions = 0;
    uint32_t diagnosticsElapsedUs = 0;

    Response responseFor(const uint8_t command) {
        switch (command) {
        case commands::READ_DEVICE_ID:
            return {deviceId, sizeof(deviceId)};
        case commands::READ_DEVICE_TYPE:
            return { &deviceType, sizeof(deviceType)};
        case commands::READ_EVENTS: {
            // Copy out: the slot can be overwritten by an enqueue while the master is reading
            const auto* event = eventQueue->peek();
            hasReadEvent = event != nullptr;
            eventResponse[0] = event ? event->type : smc::events::types::NONE;
            eventResponse[1] = event ? event->data : 0;
            return {eventResponse, sizeof(eventResponse)};
        }
        case commands::ACK_EVENT:
            if (hasReadEvent) {
                eventQueue->dequeue();
                hasReadEvent = false;
                ackResponse[0] = ackStatus::REMOVED;

                if (eventQueue->size() == 0) {
                    PORTC.OUTSET = INT_bm;
                }
            } else {
                ackResponse[0] = ackStatus::NOTHING_PENDING;
            }
            return {ackResponse, sizeof(ackResponse)};
        default:
            return {nullptr, 0};
        }
    }

    // Response bytes, then the CRC, then filler. The CRC is accumulated as the bytes are loaded.
    void loadNextByte() {
        if (responseIndex < response.length) {
            const uint8_t byte = response.data[responseIndex++];
            responseCrc = smc::utils::crc8::update(responseCrc, byte);
            SPI0.DATA = byte;
        } else if (responseIndex == response.length) {
            responseIndex++;
            SPI0.DATA = smc::utils::crc8::finalize(responseCrc);
        } else {
            SPI0.DATA = SpiSlave::FILLER_BYTE;
        }
    }

    void beginTransaction() {
        awaitingCommand = true;
        response = {nullptr, 0};
        responseIndex = 0;

        PORTC.DIRSET = MISO_bm;
    }

    void endTransaction() {
        // Release MISO for other slaves on the bus
        PORTC.DIRCLR = MISO_bm;
    }
}

void SpiSlave::setup() {
    ::eventQueue = &eventQueue;

    PORTMUX.CTRLB |= PORTMUX_SPI0_bm;

    PORTC.DIRCLR = SCK_bm | MISO_bm | MOSI_bm | SS_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

    // Set the level before enabling the output, so INT doesn't glitch low
    PORTC.OUTSET = INT_bm;
    PORTC.DIRSET = INT_bm;

    // Normal (unbuffered) mode: a written byte goes straight into the shift register and is sent
    // in the next transfer. Buffer mode is not used: it loads the shift register as the previous
    // transfer completes, before the interrupt can respond, so responses came out one byte late.
    SPI0.CTRLB = SPI_MODE_0_gc;
    SPI0.INTCTRL = SPI_IE_bm;
    SPI0.CTRLA = SPI_ENABLE_bm; // Slave (MASTER = 0), MSB first (DORD = 0)
}

void SpiSlave::update(uint32_t deltaUs, smc::events::EventQueue& queue) {
    diagnosticsElapsedUs += deltaUs;
    if (diagnosticsElapsedUs < 1000000) {
        return;
    }
    diagnosticsElapsedUs = 0;

    uint16_t falling, rising, interrupts, commands, collisions;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        falling = ssFallingEdges;
        rising = ssRisingEdges;
        interrupts = spiInterrupts;
        commands = commandsReceived;
        collisions = writeCollisions;
    }

    Serial.printf(
        "SS low: %u, SS high: %u, SPI interrupts: %u, commands: %u, write collisions: %u, SS now: %s\r\n",
        falling,
        rising,
        interrupts,
        commands,
        collisions,
        (PORTC.IN & SS_bm) ? "high" : "low"
    );
}

void SpiSlave::setDeviceId(const uint8_t (&deviceId)[identity::DEVICE_ID_LENGTH]) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        memcpy(::deviceId, deviceId, sizeof(::deviceId));
    }
}

void SpiSlave::setDeviceType(uint8_t deviceType) {
    this->deviceType = deviceType;
    ::deviceType = deviceType;
}

void SpiSlave::enqueueEvent(const events::Event& event) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // When full, the enqueue overwrites the oldest event, which is the one awaiting an ACK_EVENT
        if (eventQueue.size() == eventQueue.capacity()) {
            hasReadEvent = false;
        }

        eventQueue.enqueue(event);
        PORTC.OUTCLR = INT_bm;
    }
}

ISR(PORTC_PORT_vect) {
    const uint8_t flags = PORTC.INTFLAGS;
    PORTC.INTFLAGS = SS_bm;

    if (flags & SS_bm) {
        if (PORTC.IN & SS_bm) {
            ssRisingEdges++;
            endTransaction();
        } else {
            ssFallingEdges++;
            beginTransaction();
        }
    }
}

// Runs after every transferred byte, and loads the byte for the next transfer
ISR(SPI0_INT_vect) {
    spiInterrupts++;

    // Reading INTFLAGS, then DATA, clears IF and WRCOL
    const uint8_t flags = SPI0.INTFLAGS;
    const uint8_t received = SPI0.DATA;

    // The previous byte was written while a transfer was in progress: the master is too fast
    if (flags & SPI_WRCOL_bm) {
        writeCollisions++;
    }

    if (awaitingCommand) {
        awaitingCommand = false;
        commandsReceived++;
        responseCrc = smc::utils::crc8::update(smc::utils::crc8::INIT, received);
        response = responseFor(received);
    }

    loadNextByte();
}
