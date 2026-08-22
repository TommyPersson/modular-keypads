#pragma once

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <tfw/hal/spi/SPIBus.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::spi::test {
    struct SPIBusReadEvent {
        uint8_t address;
        uint8_t reg;
        mutable uint8_t response = 0;  // Set by observer
    };

    struct SPIBusWriteEvent {
        uint8_t address;
        uint8_t reg;
        uint8_t data;
    };

    /**
     * Stub SPIBus implementation for testing.
     * Allows tests to observe and control SPI communication.
     * Enforces initialization: read8/write8 calls are ignored until begin() is called.
     */
    class StubSPIBus : public SPIBus {
    public:
        uint8_t read8(uint8_t address, uint8_t reg) const override {
            if (!_initialized) {
                throw std::logic_error("StubSPIBus::read8() called before begin()");
            }
            SPIBusReadEvent event{.address = address, .reg = reg};
            _readSubject.notify(event);
            return event.response;
        }

        uint8_t write8(uint8_t address, uint8_t reg, uint8_t data) override {
            if (!_initialized) {
                throw std::logic_error("StubSPIBus::write8() called before begin()");
            }
            _writeSubject.notify({.address = address, .reg = reg, .data = data});
            return 0;
        }

        void begin() override {
            _initialized = true;
        }

        utils::observables::Observable<SPIBusReadEvent>& onRead() const {
            return _readSubject;
        }

        utils::observables::Observable<SPIBusWriteEvent>& onWrite() const {
            return _writeSubject;
        }

    private:
        bool _initialized = false;
        mutable utils::observables::Subject<SPIBusReadEvent> _readSubject;
        mutable utils::observables::Subject<SPIBusWriteEvent> _writeSubject;
    };
}
