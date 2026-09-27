#include "SpiSlave.h"

#include <Arduino.h>
#include <util/atomic.h>

using namespace smc::hal::spi;

// TODO verify

namespace {
    // Alternate SPI0 pins, all on PORTC
    constexpr uint8_t SCK_bm = PIN0_bm;
    constexpr uint8_t MISO_bm = PIN1_bm;
    constexpr uint8_t MOSI_bm = PIN2_bm;
    constexpr uint8_t SS_bm = PIN3_bm;

    // TODO Placeholder until the device has an ID structure
    constexpr uint8_t DEVICE_ID[] = { 0x5A, 0xC3, 0x01, 0x7E };

    struct Response {
        const uint8_t* data;
        uint8_t length;
    };

    smc::events::EventQueue* eventQueue = nullptr;

    // Transaction state, only accessed from the ISRs below
    bool awaitingCommand = true;
    Response response = { nullptr, 0 };
    uint8_t responseIndex = 0;
    uint8_t eventResponse[2] = {};

    Response responseFor(const uint8_t command) {
        switch (command) {
            case commands::READ_DEVICE_ID:
                return { DEVICE_ID, sizeof(DEVICE_ID) };
            case commands::READ_EVENTS: {
                // Copy out: the dequeued slot can be overwritten by the next enqueue
                const auto* event = eventQueue->dequeue();
                eventResponse[0] = event ? event->type : smc::events::types::NONE;
                eventResponse[1] = event ? event->data : 0;
                return { eventResponse, sizeof(eventResponse) };
            }
            default:
                return { nullptr, 0 };
        }
    }

    void loadNextByte() {
        SPI0.DATA = responseIndex < response.length ? response.data[responseIndex++] : SpiSlave::FILLER_BYTE;
    }

    void beginTransaction() {
        awaitingCommand = true;
        response = { nullptr, 0 };
        responseIndex = 0;

        PORTC.DIRSET = MISO_bm;
    }

    void endTransaction() {
        // Release MISO for other slaves on the bus
        PORTC.DIRCLR = MISO_bm;
        SPI0.INTCTRL = SPI_RXCIE_bm;

        // Discard dummy bytes the master clocked in while reading the response
        while (SPI0.INTFLAGS & SPI_RXCIF_bm) {
            (void)SPI0.DATA;
        }
        SPI0.INTFLAGS = SPI_BUFOVF_bm;
    }
}

void SpiSlave::setup() {
    ::eventQueue = &eventQueue;

    PORTMUX.CTRLB |= PORTMUX_SPI0_bm;

    PORTC.DIRCLR = SCK_bm | MISO_bm | MOSI_bm | SS_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

    // Buffer mode: a written byte waits in the TX buffer until the current transfer completes
    SPI0.CTRLB = SPI_BUFEN_bm | SPI_MODE_0_gc;
    SPI0.INTCTRL = SPI_RXCIE_bm;
    SPI0.CTRLA = SPI_ENABLE_bm;  // Slave (MASTER = 0), MSB first (DORD = 0)
}

void SpiSlave::enqueueEvent(const events::Event& event) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eventQueue.enqueue(event);
    }
}

ISR(PORTC_PORT_vect) {
    const uint8_t flags = PORTC.INTFLAGS;
    PORTC.INTFLAGS = SS_bm;

    if (flags & SS_bm) {
        if (PORTC.IN & SS_bm) {
            endTransaction();
        } else {
            beginTransaction();
        }
    }
}

ISR(SPI0_INT_vect) {
    if (SPI0.INTFLAGS & SPI_RXCIF_bm) {
        const uint8_t received = SPI0.DATA;

        if (awaitingCommand) {
            awaitingCommand = false;
            response = responseFor(received);
            loadNextByte();
            SPI0.INTCTRL = SPI_RXCIE_bm | SPI_DREIE_bm;
        }
    }

    // Keep the TX buffer one byte ahead of the master
    if ((SPI0.INTCTRL & SPI_DREIE_bm) && (SPI0.INTFLAGS & SPI_DREIF_bm)) {
        loadNextByte();
    }
}
