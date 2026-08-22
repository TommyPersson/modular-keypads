#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <tfw/utils/observables.h>
#include "../test_hal/StubInputPin.h"
#include "../test_hal/StubOutputPin.h"

namespace tfw::ic::test {
    /**
     * Emulator for the 74165 Parallel-In, Serial-Out Shift Register IC.
     *
     * This emulator simulates the behavior of the 74165 shift register, allowing
     * tests to control the parallel inputs and observe serial output behavior.
     *
     * The emulator:
     * - Subscribes to output pin state changes (LD, CLK, CE)
     * - Updates Q output based on pin state transitions
     * - Models parallel load on LD rising edge
     * - Models serial shift on CLK falling edge when CE is LOW
     */
    class L74165Emulator : public tfw::utils::observables::Observer<tfw::hal::gpio::test::OutputPinStateChangedEvent> {
    public:
        /**
         * Create an emulator connected to the specified pins.
         * The emulator subscribes to output pin state changes to track control signals.
         * Pins must remain valid for the lifetime of this emulator.
         */
        L74165Emulator(
            tfw::hal::gpio::test::StubInputPin& pinQ,
            tfw::hal::gpio::test::StubOutputPin& pinCLK,
            tfw::hal::gpio::test::StubOutputPin& pinCE,
            tfw::hal::gpio::test::StubOutputPin& pinLD
        ) : _parallelData(0),
            _shiftRegister(0),
            _ldPrevious(1),
            _clkPrevious(1),
            _cePrevious(1),
            _shiftPosition(7),
            _pinQ(pinQ),
            _pinCLK(pinCLK),
            _pinCE(pinCE),
            _pinLD(pinLD) {
            _pinCLK.onStateChanged().addObserver(this);
            _pinCE.onStateChanged().addObserver(this);
            _pinLD.onStateChanged().addObserver(this);
        }

        /**
         * Observer method called when any subscribed pin state changes.
         * Updates emulator state based on the pin change and validates timing constraints.
         */
        void observe(const tfw::hal::gpio::test::OutputPinStateChangedEvent& event) override {
            if (event.pinNumber == _pinLD.pinNumber) {
                validateLDTiming(event);
                handleLDChange(event.state);
            } else if (event.pinNumber == _pinCLK.pinNumber) {
                validateCLKTiming(event);
                handleCLKChange(event.state);
            } else if (event.pinNumber == _pinCE.pinNumber) {
                handleCEChange(event.state);
            }
        }

        /**
         * Set the parallel input data (D0-D7).
         * This is what gets loaded into the shift register on the next parallel load.
         */
        void setParallelData(uint8_t data) {
            _parallelData = data;
        }

    private:
        /**
         * Validate LD pin timing constraints.
         * 74HC165 datasheet specifies minimum pulse width of ~15ns.
         * We enforce 50ns (3.3x margin) to catch missing/insufficient delays.
         */
        void validateLDTiming(const tfw::hal::gpio::test::OutputPinStateChangedEvent& event) {
            // Minimum LD pulse width: 50ns (74HC165 typical is ~15ns)
            constexpr uint64_t MIN_LD_PULSE_WIDTH_NS = 50;

            if (_ldPrevious == 1 && event.state == 0) {
                // LD going LOW - record the time
                _ldLowTimestampNs = event.timestampNs;
            } else if (_ldPrevious == 0 && event.state == 1) {
                // LD going HIGH - validate pulse width
                if (_ldLowTimestampNs > 0) {
                    uint64_t pulseWidth = event.timestampNs - _ldLowTimestampNs;
                    if (pulseWidth < MIN_LD_PULSE_WIDTH_NS) {
                        throw std::logic_error(
                            "L74165Emulator: LD pulse width too short (" +
                            std::to_string(pulseWidth) + "ns < " +
                            std::to_string(MIN_LD_PULSE_WIDTH_NS) + "ns). "
                            "74HC165 requires minimum 15ns (using 50ns margin)."
                        );
                    }
                }
            }
        }

        /**
         * Validate CLK pin timing constraints.
         * 74HC165 datasheet specifies minimum pulse width of ~15ns.
         * We enforce 50ns (3.3x margin) to catch missing/insufficient delays.
         */
        void validateCLKTiming(const tfw::hal::gpio::test::OutputPinStateChangedEvent& event) {
            // Minimum CLK pulse width: 50ns (74HC165 typical is ~15ns)
            constexpr uint64_t MIN_CLK_PULSE_WIDTH_NS = 50;

            if (_clkPrevious == 1 && event.state == 0) {
                // CLK going LOW - record the time
                _clkLowTimestampNs = event.timestampNs;
            } else if (_clkPrevious == 0 && event.state == 1) {
                // CLK going HIGH - validate pulse width
                if (_clkLowTimestampNs > 0) {
                    uint64_t pulseWidth = event.timestampNs - _clkLowTimestampNs;
                    if (pulseWidth < MIN_CLK_PULSE_WIDTH_NS) {
                        throw std::logic_error(
                            "L74165Emulator: CLK pulse width too short (" +
                            std::to_string(pulseWidth) + "ns < " +
                            std::to_string(MIN_CLK_PULSE_WIDTH_NS) + "ns). "
                            "74HC165 requires minimum 15ns (using 50ns margin)."
                        );
                    }
                }
            }
        }

        /**
         * Handle LD (load) pin state change.
         * Detects rising edge and loads parallel data.
         */
        void handleLDChange(uint8_t ldState) {
            if (_ldPrevious == 0 && ldState == 1) {
                _shiftRegister = _parallelData;
                _shiftPosition = 7;
                uint8_t qOutput = (_shiftRegister >> 7) & 0x01;
                _pinQ.setValue(qOutput);
            }
            _ldPrevious = ldState;
        }

        /**
         * Handle CLK (clock) pin state change.
         * Detects falling edge and shifts data when CE is LOW.
         */
        void handleCLKChange(uint8_t clkState) {
            if (_clkPrevious == 1 && clkState == 0) {
                processClockEdge();
            }
            _clkPrevious = clkState;
        }

        /**
         * Handle CE (chip enable) pin state change.
         */
        void handleCEChange(uint8_t ceState) {
            _cePrevious = ceState;
        }

        /**
         * Process a clock edge during serial shifting.
         * Shifts to next bit if CE is LOW.
         */
        void processClockEdge() {
            if (_cePrevious == 0) {
                if (_shiftPosition > 0) {
                    _shiftPosition--;
                }
                uint8_t qOutput = (_shiftRegister >> _shiftPosition) & 0x01;
                _pinQ.setValue(qOutput);
            }
        }

        uint8_t _parallelData; // D0-D7 parallel inputs
        uint8_t _shiftRegister; // Internal shift register
        uint8_t _ldPrevious; // Previous state of LD for edge detection
        uint8_t _clkPrevious; // Previous state of CLK for edge detection
        uint8_t _cePrevious; // Previous state of CE
        int _shiftPosition; // Current bit position (7 = MSB, 0 = LSB)

        // Timing tracking for constraint validation (in nanoseconds)
        uint64_t _ldLowTimestampNs = 0; // Timestamp when LD went LOW
        uint64_t _clkLowTimestampNs = 0; // Timestamp when CLK went LOW

        // Pin references (guaranteed to be valid for emulator lifetime)
        tfw::hal::gpio::test::StubInputPin& _pinQ;
        tfw::hal::gpio::test::StubOutputPin& _pinCLK;
        tfw::hal::gpio::test::StubOutputPin& _pinCE;
        tfw::hal::gpio::test::StubOutputPin& _pinLD;
    };
}
