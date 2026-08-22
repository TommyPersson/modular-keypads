#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <stdexcept>
#include <tfw/utils/observables.h>
#include "../test_hal/StubSPIBus.h"
#include "../test_hal/StubOutputPin.h"

namespace tfw::ic::test {
    /**
     * Emulator for the MCP23x17 SPI-based I/O expander.
     *
     * Simulates the internal register behavior of the MCP23x17 device.
     * Observes read/write operations on the stub serial bus and provides
     * appropriate responses based on internal register state.
     */
    class MCP23x17Emulator : public tfw::utils::observables::Observer<tfw::hal::spi::test::SPIBusWriteEvent>,
                              public tfw::utils::observables::Observer<tfw::hal::spi::test::SPIBusReadEvent>,
                              public tfw::utils::observables::Observer<tfw::hal::gpio::test::OutputPinStateChangedEvent> {
    public:
        // Register addresses
        static constexpr uint8_t IODIRA = 0x00;
        static constexpr uint8_t IODIRB = 0x01;
        static constexpr uint8_t IOPOLA = 0x02;
        static constexpr uint8_t IOPOLB = 0x03;
        static constexpr uint8_t GPPUA = 0x0c;
        static constexpr uint8_t GPPUB = 0x0d;
        static constexpr uint8_t GPIOA = 0x12;
        static constexpr uint8_t GPIOB = 0x13;

        /**
         * Create an emulator connected to the specified serial bus and reset pin.
         * The emulator enforces proper initialization: read/write operations only
         * succeed after a valid reset sequence (LOW then HIGH on reset pin).
         */
        MCP23x17Emulator(tfw::hal::spi::test::StubSPIBus& bus,
                         tfw::hal::gpio::test::StubOutputPin& resetPin)
            : _bus(bus), _resetPin(resetPin) {
            _bus.onWrite().addObserver(this);
            _bus.onRead().addObserver(this);
            _resetPin.onStateChanged().addObserver(this);
        }

        /**
         * Observer method called when a write occurs on the serial bus.
         * Updates emulator state based on the write operation.
         * Writes only succeed after proper reset initialization sequence.
         */
        void observe(const tfw::hal::spi::test::SPIBusWriteEvent& event) override {
            if (event.address == 0x00) {  // Default MCP23x17 address
                if (!_resetComplete) {
                    throw std::logic_error("MCP23x17Emulator: write attempted before reset sequence completed");
                }
                handleRegisterWrite(event.reg, event.data);
            }
        }

        /**
         * Observer method called when a read occurs on the serial bus.
         * Sets the response value based on current emulator state.
         * Throws if device has not been properly initialized via reset sequence.
         */
        void observe(const tfw::hal::spi::test::SPIBusReadEvent& event) override {
            if (event.address == 0x00) {  // Default MCP23x17 address
                if (!_resetComplete) {
                    throw std::logic_error("MCP23x17Emulator: read attempted before reset sequence completed");
                }
                event.response = handleRegisterRead(event.reg);
            }
        }

        /**
         * Observer method called when the reset pin state changes.
         * Tracks the reset sequence: LOW then HIGH completes initialization.
         * Validates timing constraints on reset pulse.
         */
        void observe(const tfw::hal::gpio::test::OutputPinStateChangedEvent& event) override {
            if (event.pinNumber == _resetPin.pinNumber) {
                if (event.state == 0 && !_resetInProgress) {
                    // Falling edge: reset sequence started
                    _resetLowTimestampNs = event.timestampNs;
                    _resetLowWasRecorded = true;
                    _resetInProgress = true;
                    _resetComplete = false;
                } else if (event.state == 1 && _resetInProgress) {
                    // Rising edge after falling edge: reset sequence complete
                    validateResetTiming(event.timestampNs);
                    _resetComplete = true;
                    _resetInProgress = false;
                }
            }
        }

        /**
         * Set the port input value (what would be read from GPIO pins).
         */
        void setPortAInput(uint8_t value) {
            _registers[GPIOA] = value;
        }

        void setPortBInput(uint8_t value) {
            _registers[GPIOB] = value;
        }

        /**
         * Get a register value (as the device would return it on read).
         */
        uint8_t getRegister(uint8_t reg) const {
            if (reg < _registers.size()) {
                return _registers[reg];
            }
            return 0;
        }

    private:
        /**
         * Validate reset pin pulse timing.
         * MCP23x17 datasheet specifies minimum reset pulse width of ~1µs.
         * We enforce 500ns to catch missing delays while allowing safety margin.
         */
        void validateResetTiming(uint64_t resetHighTimestampNs) {
            // Minimum reset pulse width: 1µs (matches MCP23x17 datasheet spec)
            constexpr uint64_t MIN_RESET_PULSE_WIDTH_NS = 1000;  // 1µs in nanoseconds

            // Check if LOW was recorded (using UINT64_MAX as sentinel since timestamp can be 0)
            if (_resetLowWasRecorded) {
                uint64_t pulseWidth = resetHighTimestampNs - _resetLowTimestampNs;
                if (pulseWidth < MIN_RESET_PULSE_WIDTH_NS) {
                    throw std::logic_error(
                        "MCP23x17Emulator: RESET pulse width too short (" +
                        std::to_string(pulseWidth / 1000) + "µs < " +
                        std::to_string(MIN_RESET_PULSE_WIDTH_NS / 1000) + "µs). "
                        "MCP23x17 requires minimum 1µs reset pulse."
                    );
                }
            }
        }

        uint8_t handleRegisterRead(uint8_t reg) {
            if (reg >= _registers.size()) {
                return 0;
            }

            uint8_t value = _registers[reg];

            // Apply polarity inversion for GPIO port reads
            if (reg == GPIOA && _registers[IOPOLA]) {
                value ^= _registers[IOPOLA];
            } else if (reg == GPIOB && _registers[IOPOLB]) {
                value ^= _registers[IOPOLB];
            }

            return value;
        }

        void handleRegisterWrite(uint8_t reg, uint8_t data) {
            if (reg < _registers.size()) {
                _registers[reg] = data;
            }
        }

        // Register storage (16 bytes for MCP23x17)
        // GPIOA and GPIOB store the external pin input values
        std::array<uint8_t, 0x16> _registers{};

        tfw::hal::spi::test::StubSPIBus& _bus;
        tfw::hal::gpio::test::StubOutputPin& _resetPin;

        // Reset sequence tracking (timestamps in nanoseconds)
        bool _resetInProgress = false;
        bool _resetComplete = false;
        bool _resetLowWasRecorded = false;  // Flag to detect when LOW was recorded (since timestamp can be 0)
        uint64_t _resetLowTimestampNs = 0;  // Timestamp when reset pin went LOW
    };
}
