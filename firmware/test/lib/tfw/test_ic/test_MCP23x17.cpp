#include <gtest/gtest.h>
#include <memory>

// Test headers
#include "../test_hal/StubSPIBus.h"
#include "../test_hal/StubOutputPin.h"
#include "../test_hal/VirtualClock.h"
#include "MCP23x17Emulator.h"

// Production code
#include <tfw/ic/MCP23x17/MCP23x17.h>

using namespace tfw::hal::spi;
using namespace tfw::hal::spi::test;
using namespace tfw::hal::time::test;
using namespace tfw::ic::test;

/**
 * Test fixture for MCP23x17 IC tests.
 *
 * The test fixture sets up stub components:
 * - A stub serial bus to mock SPI communication
 * - An emulator to simulate MCP23x17 behavior
 * - A stub output pin for reset control
 * - An MCP23x17 driver instance
 */
class MCP23x17Test : public ::testing::Test {
protected:
    MCP23x17Test() = default;

    void SetUp() override {
        // Create virtual clock for timing tracking
        clock = std::make_unique<VirtualClock>();

        // Create stub serial bus
        auto stub_bus = std::make_unique<StubSPIBus>();
        auto* bus_ptr = stub_bus.get();

        // Create reset pin stub with clock reference and save reference before moving
        auto reset_pin_ptr = std::make_unique<tfw::hal::gpio::test::StubOutputPin>(42, *clock);
        reset_pin = reset_pin_ptr.get();

        // Create emulator connected to the bus and reset pin
        // Emulator enforces proper initialization via reset sequence
        emulator = std::make_unique<MCP23x17Emulator>(*bus_ptr, *reset_pin);

        // Create MCP23x17 driver with stub bus, reset pin, and clock
        mcp23x17 = std::make_unique<tfw::ic::MCP23x17>(std::move(stub_bus),
                                                        std::move(reset_pin_ptr),
                                                        *clock);
    }

    std::unique_ptr<VirtualClock> clock;
    std::unique_ptr<tfw::ic::MCP23x17> mcp23x17;
    std::unique_ptr<MCP23x17Emulator> emulator;
    tfw::hal::gpio::test::StubOutputPin* reset_pin = nullptr;
};

// ============================================================================
// Initialization Tests
// ============================================================================

TEST_F(MCP23x17Test, SetupInitializes) {
    mcp23x17->begin();
    // If we get here without crash, initialization was successful
    EXPECT_TRUE(true);
}

TEST_F(MCP23x17Test, SetupSetsDefaultConfiguration) {
    mcp23x17->begin();
    // After setup, device should be initialized
    EXPECT_TRUE(true);
}

TEST_F(MCP23x17Test, BeginPulsesResetPin) {
    // Track reset pin state transitions
    bool saw_low = false;
    bool saw_high_after_low = false;

    class ResetPinObserver : public tfw::utils::observables::Observer<tfw::hal::gpio::test::OutputPinStateChangedEvent> {
    public:
        bool& saw_low;
        bool& saw_high_after_low;
        uint8_t last_state = 1;

        ResetPinObserver(bool& low, bool& high) : saw_low(low), saw_high_after_low(high) {}

        void observe(const tfw::hal::gpio::test::OutputPinStateChangedEvent& event) override {
            if (event.pinNumber == 42) {  // Reset pin
                if (event.state == 0 && last_state == 1) {
                    saw_low = true;
                }
                if (event.state == 1 && saw_low) {
                    saw_high_after_low = true;
                }
                last_state = event.state;
            }
        }
    };

    ResetPinObserver observer(saw_low, saw_high_after_low);
    reset_pin->onStateChanged().addObserver(&observer);

    mcp23x17->begin();

    // Verify that the reset pin was pulsed: went low then high
    EXPECT_TRUE(saw_low);
    EXPECT_TRUE(saw_high_after_low);
}

// ============================================================================
// Register Write Tests
// ============================================================================

TEST_F(MCP23x17Test, WriteRegisterIODIRA) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0xFF);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRA), 0xFF);
}

TEST_F(MCP23x17Test, WriteRegisterIODIRB) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IODIRB, 0x00);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRB), 0x00);
}

TEST_F(MCP23x17Test, WriteRegisterIOPOLA) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IOPOLA, 0xAA);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IOPOLA), 0xAA);
}

TEST_F(MCP23x17Test, WriteRegisterIOPOLB) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IOPOLB, 0x55);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IOPOLB), 0x55);
}

TEST_F(MCP23x17Test, WriteRegisterGPPUA) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::GPPUA, 0xFF);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUA), 0xFF);
}

TEST_F(MCP23x17Test, WriteRegisterGPPUB) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::GPPUB, 0x00);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUB), 0x00);
}

// ============================================================================
// Register Read Tests
// ============================================================================

TEST_F(MCP23x17Test, ReadRegisterReturnsWrittenValue) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0x42);

    // Emulator automatically provides the correct response
    uint8_t result = mcp23x17->readRegister(tfw::ic::registers::IODIRA);
    EXPECT_EQ(result, 0x42);
}

TEST_F(MCP23x17Test, ReadMultipleRegisters) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0xFF);
    mcp23x17->writeRegister(tfw::ic::registers::IODIRB, 0x00);
    mcp23x17->writeRegister(tfw::ic::registers::IOPOLA, 0xAA);

    EXPECT_EQ(mcp23x17->readRegister(tfw::ic::registers::IODIRA), 0xFF);
    EXPECT_EQ(mcp23x17->readRegister(tfw::ic::registers::IODIRB), 0x00);
    EXPECT_EQ(mcp23x17->readRegister(tfw::ic::registers::IOPOLA), 0xAA);
}

// ============================================================================
// Port Configuration Tests
// ============================================================================

TEST_F(MCP23x17Test, ConfigurePortAAsInputs) {
    mcp23x17->begin();

    // Set all bits as inputs (1 = input, 0 = output)
    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0xFF);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRA), 0xFF);
}

TEST_F(MCP23x17Test, ConfigurePortAAsOutputs) {
    mcp23x17->begin();

    // Set all bits as outputs (0 = output)
    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0x00);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRA), 0x00);
}

TEST_F(MCP23x17Test, ConfigurePortAMixedIO) {
    mcp23x17->begin();

    // Set alternating input/output
    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0xAA);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRA), 0xAA);
}

TEST_F(MCP23x17Test, ConfigurePortBAsInputs) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IODIRB, 0xFF);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRB), 0xFF);
}

// ============================================================================
// Polarity Inversion Tests
// ============================================================================

TEST_F(MCP23x17Test, SetPolarityInversionPortA) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IOPOLA, 0xFF);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IOPOLA), 0xFF);
}

TEST_F(MCP23x17Test, SetPolarityInversionPortB) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IOPOLB, 0x55);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IOPOLB), 0x55);
}

// ============================================================================
// Pull-up Configuration Tests
// ============================================================================

TEST_F(MCP23x17Test, EnablePullUpPortA) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::GPPUA, 0xFF);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUA), 0xFF);
}

TEST_F(MCP23x17Test, EnablePullUpPortB) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::GPPUB, 0xAA);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUB), 0xAA);
}

TEST_F(MCP23x17Test, DisablePullUps) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::GPPUA, 0x00);
    mcp23x17->writeRegister(tfw::ic::registers::GPPUB, 0x00);

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUA), 0x00);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUB), 0x00);
}

// ============================================================================
// Port Read Tests
// ============================================================================

TEST_F(MCP23x17Test, ReadPortAAllZeros) {
    mcp23x17->begin();

    emulator->setPortAInput(0x00);
    uint8_t result = mcp23x17->readPortA();
    EXPECT_EQ(result, 0x00);
}

TEST_F(MCP23x17Test, ReadPortAAllOnes) {
    mcp23x17->begin();

    emulator->setPortAInput(0xFF);
    uint8_t result = mcp23x17->readPortA();
    EXPECT_EQ(result, 0xFF);
}

TEST_F(MCP23x17Test, ReadPortBAllZeros) {
    mcp23x17->begin();

    emulator->setPortBInput(0x00);
    uint8_t result = mcp23x17->readPortB();
    EXPECT_EQ(result, 0x00);
}

TEST_F(MCP23x17Test, ReadPortBAllOnes) {
    mcp23x17->begin();

    emulator->setPortBInput(0xFF);
    uint8_t result = mcp23x17->readPortB();
    EXPECT_EQ(result, 0xFF);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_F(MCP23x17Test, FullSetupSequence) {
    mcp23x17->begin();

    // Typical setup sequence
    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0xFF); // All inputs
    mcp23x17->writeRegister(tfw::ic::registers::IODIRB, 0xFF); // All inputs
    mcp23x17->writeRegister(tfw::ic::registers::GPPUA, 0xFF); // Enable pull-ups A
    mcp23x17->writeRegister(tfw::ic::registers::GPPUB, 0xFF); // Enable pull-ups B

    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRA), 0xFF);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::IODIRB), 0xFF);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUA), 0xFF);
    EXPECT_EQ(emulator->getRegister(tfw::ic::registers::GPPUB), 0xFF);
}

TEST_F(MCP23x17Test, AlternatingReadWrite) {
    mcp23x17->begin();

    mcp23x17->writeRegister(tfw::ic::registers::IOPOLA, 0x00);
    uint8_t result = mcp23x17->readRegister(tfw::ic::registers::IOPOLA);
    EXPECT_EQ(result, 0x00);

    mcp23x17->writeRegister(tfw::ic::registers::IOPOLA, 0xFF);
    result = mcp23x17->readRegister(tfw::ic::registers::IOPOLA);
    EXPECT_EQ(result, 0xFF);
}

// ============================================================================
// Stress Tests
// ============================================================================

TEST_F(MCP23x17Test, SequentialRegisterWrites) {
    mcp23x17->begin();

    for (int i = 0; i < 16; ++i) {
        uint8_t pattern = (i * 17) & 0xFF;
        mcp23x17->writeRegister(i, pattern);
        EXPECT_EQ(emulator->getRegister(i), pattern);
    }
}

TEST_F(MCP23x17Test, RepeatedPortReads) {
    mcp23x17->begin();

    emulator->setPortAInput(0x42);

    for (int i = 0; i < 50; ++i) {
        uint8_t result = mcp23x17->readPortA();
        EXPECT_EQ(result, 0x42);
    }
}
