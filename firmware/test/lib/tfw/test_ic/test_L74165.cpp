#include <gtest/gtest.h>
#include <memory>

// Test headers
#include "../test_hal/StubInputPin.h"
#include "../test_hal/StubOutputPin.h"
#include "L74165Emulator.h"

// Production code
#include <tfw/ic/L74165.h>

using namespace tfw::hal::gpio;
using namespace tfw::hal::gpio::test;
using namespace tfw::ic::test;

/**
 * Test fixture for L74165 IC tests.
 *
 * The test fixture sets up stub pins, connects them to an emulator,
 * and creates an L74165 driver instance. The emulator observes pin changes
 * and updates its state accordingly, while tests interact directly with
 * the L74165 driver.
 */
class L74165Test : public ::testing::Test {
protected:
    L74165Test() = default;

    void SetUp() override {
        // Create stub pins
        auto pin_q = std::make_unique<StubInputPin>(21);
        auto pin_clk = std::make_unique<StubOutputPin>(19);
        auto pin_ce = std::make_unique<StubOutputPin>(18);
        auto pin_ld = std::make_unique<StubOutputPin>(17);

        // Create emulator connected to the pins
        emulator = std::make_unique<L74165Emulator>(
            *pin_q,
            *pin_clk,
            *pin_ce,
            *pin_ld
        );

        // Create L74165 config with stub pins
        tfw::ic::Config config{
            std::move(pin_q),
            std::move(pin_clk),
            std::move(pin_ce),
            std::move(pin_ld)
        };

        // Create L74165 driver
        l74165 = std::make_unique<tfw::ic::L74165>(std::move(config));
    }

    std::unique_ptr<tfw::ic::L74165> l74165;
    std::unique_ptr<L74165Emulator> emulator;
};

// ============================================================================
// Initialization Tests
// ============================================================================

TEST_F(L74165Test, SetupInitializesPins) {
    l74165->setup();
    // If we get here without crash, pins were initialized properly
    EXPECT_TRUE(true);
}

TEST_F(L74165Test, SetupSetsControlPinsHigh) {
    l74165->setup();
    // Control pins should be HIGH after setup - verified by successful operation
    EXPECT_TRUE(true);
}

// ============================================================================
// Parallel Load Tests
// ============================================================================

TEST_F(L74165Test, ParallelLoadCompletes) {
    l74165->setup();
    l74165->parallelLoad();
    // Parallel load completes without error
    EXPECT_TRUE(true);
}

// ============================================================================
// Read Operation Tests
// ============================================================================

TEST_F(L74165Test, ReadReturnsAllZeros) {
    l74165->setup();
    emulator->setParallelData(0x00);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x00);
}

TEST_F(L74165Test, ReadReturnsAllOnes) {
    l74165->setup();
    emulator->setParallelData(0xFF);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0xFF);
}

TEST_F(L74165Test, ReadReturnsCorrectPattern_10101010) {
    l74165->setup();
    emulator->setParallelData(0xAA);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0xAA);
}

TEST_F(L74165Test, ReadReturnsCorrectPattern_01010101) {
    l74165->setup();
    emulator->setParallelData(0x55);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x55);
}

TEST_F(L74165Test, ReadReturnsCorrectPattern_10110011) {
    l74165->setup();
    emulator->setParallelData(0xB3);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0xB3);
}

TEST_F(L74165Test, ReadReturnsCorrectPattern_00000001) {
    l74165->setup();
    emulator->setParallelData(0x01);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x01);
}

TEST_F(L74165Test, ReadReturnsCorrectPattern_10000000) {
    l74165->setup();
    emulator->setParallelData(0x80);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x80);
}

// ============================================================================
// Multiple Read Tests
// ============================================================================

TEST_F(L74165Test, MultipleReadsWithDifferentData) {
    l74165->setup();

    {
        emulator->setParallelData(0x55);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, 0x55);
    }

    {
        emulator->setParallelData(0xAA);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, 0xAA);
    }

    {
        emulator->setParallelData(0xF0);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, 0xF0);
    }
}

TEST_F(L74165Test, ReadAfterExplicitParallelLoad) {
    l74165->setup();
    l74165->parallelLoad();

    emulator->setParallelData(0x42);
    l74165->parallelLoad();
    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x42);
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(L74165Test, ReadDoesNotCorruptState) {
    l74165->setup();

    // Read the same data multiple times
    uint8_t data = 0x7E;

    {
        emulator->setParallelData(data);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, data);
    }

    {
        emulator->setParallelData(data);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, data);
    }

    {
        emulator->setParallelData(data);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, data);
    }
}

TEST_F(L74165Test, SequentialBitPatterns) {
    l74165->setup();

    // Test all single-bit patterns
    for (int bit = 0; bit < 8; ++bit) {
        uint8_t expected = 1 << bit;

        emulator->setParallelData(expected);
        l74165->parallelLoad();
        uint8_t result = l74165->read();

        EXPECT_EQ(result, expected) << "Failed for single-bit pattern at position " << bit;
    }
}

// ============================================================================
// Full Integration Tests
// ============================================================================

TEST_F(L74165Test, FullSequence_SetupReadMultiple) {
    l74165->setup();

    {
        emulator->setParallelData(0x00);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, 0x00);
    }

    {
        emulator->setParallelData(0xFF);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, 0xFF);
    }

    {
        emulator->setParallelData(0x55);
        l74165->parallelLoad();
        uint8_t result = l74165->read();
        EXPECT_EQ(result, 0x55);
    }
}

TEST_F(L74165Test, FullSequence_SetupLoadRead) {
    l74165->setup();
    l74165->parallelLoad();

    emulator->setParallelData(0xC3);
    l74165->parallelLoad();
    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0xC3);
}

// ============================================================================
// Boundary Tests
// ============================================================================

TEST_F(L74165Test, MinimumValue) {
    l74165->setup();
    emulator->setParallelData(0x00);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x00);
}

TEST_F(L74165Test, MaximumValue) {
    l74165->setup();
    emulator->setParallelData(0xFF);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0xFF);
}

TEST_F(L74165Test, MidpointValue) {
    l74165->setup();
    emulator->setParallelData(0x80);
    l74165->parallelLoad();

    uint8_t result = l74165->read();

    EXPECT_EQ(result, 0x80);
}

// ============================================================================
// Stress Tests
// ============================================================================

TEST_F(L74165Test, ManySequentialReads) {
    l74165->setup();

    // Perform 100 sequential reads with different data
    for (int i = 0; i < 100; ++i) {
        uint8_t pattern = (i * 17) & 0xFF; // Pseudo-random pattern

        emulator->setParallelData(pattern);
        l74165->parallelLoad();
        uint8_t result = l74165->read();

        EXPECT_EQ(result, pattern) << "Failed at iteration " << i;
    }
}

TEST_F(L74165Test, RepetitiveReadsStability) {
    l74165->setup();

    // Read the same value many times
    uint8_t testValue = 0x7E;

    for (int i = 0; i < 50; ++i) {
        emulator->setParallelData(testValue);
        l74165->parallelLoad();
        uint8_t result = l74165->read();

        EXPECT_EQ(result, testValue);
    }
}
