#include <gtest/gtest.h>
#include <limits>
#include "tfw/utils/strings.h"

using namespace tfw::utils::strings;

// Test fixture for string conversion tests
class StringsTest : public ::testing::Test {
protected:
    // Helper to safely access buffer contents after decode
    std::string bufferToString(const char* buffer) {
        return std::string(buffer);
    }
};

// ============================================================================
// atol() - String to signed long tests
// ============================================================================

TEST_F(StringsTest, AtoL_DecimalPositive) {
    EXPECT_EQ(atol("123"), 123);
    EXPECT_EQ(atol("0"), 0);
    EXPECT_EQ(atol("999999"), 999999);
}

TEST_F(StringsTest, AtoL_DecimalNegative) {
    EXPECT_EQ(atol("-123"), -123);
    EXPECT_EQ(atol("-1"), -1);
}

TEST_F(StringsTest, AtoL_Hex) {
    EXPECT_EQ(atol("FF", 16), 255);
    EXPECT_EQ(atol("10", 16), 16);
    EXPECT_EQ(atol("ABC", 16), 2748);
}

TEST_F(StringsTest, AtoL_Octal) {
    EXPECT_EQ(atol("10", 8), 8);
    EXPECT_EQ(atol("77", 8), 63);
    EXPECT_EQ(atol("123", 8), 83);
}

TEST_F(StringsTest, AtoL_Binary) {
    EXPECT_EQ(atol("1010", 2), 10);
    EXPECT_EQ(atol("1111", 2), 15);
    EXPECT_EQ(atol("100000", 2), 32);
}

TEST_F(StringsTest, AtoL_EmptyString) {
    EXPECT_EQ(atol(""), 0);
}

TEST_F(StringsTest, AtoL_WithLeadingZeros) {
    EXPECT_EQ(atol("00123"), 123);
    EXPECT_EQ(atol("0000"), 0);
}

// ============================================================================
// atou8() - String to uint8_t tests
// ============================================================================

TEST_F(StringsTest, AtoU8_DecimalValid) {
    EXPECT_EQ(atou8("0"), 0);
    EXPECT_EQ(atou8("1"), 1);
    EXPECT_EQ(atou8("255"), 255);
    EXPECT_EQ(atou8("128"), 128);
}

TEST_F(StringsTest, AtoU8_Hex) {
    EXPECT_EQ(atou8("FF", 16), 255);
    EXPECT_EQ(atou8("00", 16), 0);
    EXPECT_EQ(atou8("80", 16), 128);
}

TEST_F(StringsTest, AtoU8_Octal) {
    EXPECT_EQ(atou8("377", 8), 255);  // 11111111 in binary
    EXPECT_EQ(atou8("200", 8), 128);  // 10000000 in binary
}

TEST_F(StringsTest, AtoU8_Overflow) {
    // Values beyond uint8_t range wrap around
    uint8_t result = atou8("256");  // 256 % 256 = 0
    EXPECT_EQ(result, 0);

    result = atou8("257");  // 257 % 256 = 1
    EXPECT_EQ(result, 1);
}

// ============================================================================
// atou16() - String to uint16_t tests
// ============================================================================

TEST_F(StringsTest, AtoU16_DecimalValid) {
    EXPECT_EQ(atou16("0"), 0);
    EXPECT_EQ(atou16("1"), 1);
    EXPECT_EQ(atou16("65535"), 65535);
    EXPECT_EQ(atou16("32768"), 32768);
}

TEST_F(StringsTest, AtoU16_Hex) {
    EXPECT_EQ(atou16("FFFF", 16), 65535);
    EXPECT_EQ(atou16("1000", 16), 4096);
    EXPECT_EQ(atou16("8000", 16), 32768);
}

TEST_F(StringsTest, AtoU16_LargeValues) {
    EXPECT_EQ(atou16("12345"), 12345);
    EXPECT_EQ(atou16("65534"), 65534);
}

// ============================================================================
// atou32() - String to uint32_t tests
// ============================================================================

TEST_F(StringsTest, AtoU32_DecimalValid) {
    EXPECT_EQ(atou32("0"), 0U);
    EXPECT_EQ(atou32("1"), 1U);
    EXPECT_EQ(atou32("4294967295"), 4294967295U);  // Max uint32_t
    EXPECT_EQ(atou32("1000000"), 1000000U);
}

TEST_F(StringsTest, AtoU32_Hex) {
    EXPECT_EQ(atou32("FFFFFFFF", 16), 4294967295U);
    EXPECT_EQ(atou32("12345678", 16), 0x12345678U);
    EXPECT_EQ(atou32("80000000", 16), 2147483648U);
}

TEST_F(StringsTest, AtoU32_LargeHexValues) {
    EXPECT_EQ(atou32("DEADBEEF", 16), 0xDEADBEEFU);
    EXPECT_EQ(atou32("CAFEBABE", 16), 0xCAFEBABEU);
}

// ============================================================================
// atou64() - String to uint64_t tests
// ============================================================================

TEST_F(StringsTest, AtoU64_DecimalValid) {
    EXPECT_EQ(atou64("0"), 0ULL);
    EXPECT_EQ(atou64("1"), 1ULL);
    EXPECT_EQ(atou64("18446744073709551615"), 18446744073709551615ULL);  // Max uint64_t
}

TEST_F(StringsTest, AtoU64_LargeValues) {
    EXPECT_EQ(atou64("1000000000000"), 1000000000000ULL);
    EXPECT_EQ(atou64("9999999999999999"), 9999999999999999ULL);
}

TEST_F(StringsTest, AtoU64_Hex) {
    EXPECT_EQ(atou64("FFFFFFFFFFFFFFFF", 16), 18446744073709551615ULL);
    EXPECT_EQ(atou64("0123456789ABCDEF", 16), 0x0123456789ABCDEFULL);
}

// ============================================================================
// uriDecode() - URI percent-encoding tests
// ============================================================================

TEST_F(StringsTest, UriDecode_NoEncoding) {
    char buffer[100];
    size_t len = uriDecode("hello", buffer, sizeof(buffer));
    EXPECT_EQ(len, 5);
    EXPECT_STREQ(buffer, "hello");
}

TEST_F(StringsTest, UriDecode_SinglePercentEncoded) {
    char buffer[100];
    size_t len = uriDecode("hello%20world", buffer, sizeof(buffer));
    EXPECT_EQ(len, 11);
    EXPECT_STREQ(buffer, "hello world");
}

TEST_F(StringsTest, UriDecode_MultiplePercentEncoded) {
    char buffer[100];
    size_t len = uriDecode("a%20b%20c", buffer, sizeof(buffer));
    EXPECT_EQ(len, 5);
    EXPECT_STREQ(buffer, "a b c");
}

TEST_F(StringsTest, UriDecode_SpecialCharacters) {
    char buffer[100];

    // %2F = /
    size_t len = uriDecode("path%2Fto%2Ffile", buffer, sizeof(buffer));
    EXPECT_EQ(len, 12);
    EXPECT_STREQ(buffer, "path/to/file");

    // %3D = =
    len = uriDecode("key%3Dvalue", buffer, sizeof(buffer));
    EXPECT_EQ(len, 9);
    EXPECT_STREQ(buffer, "key=value");
}

TEST_F(StringsTest, UriDecode_HexCharacters) {
    char buffer[100];

    // Test hex digit conversion (0-F)
    size_t len = uriDecode("%2F%3F%3D", buffer, sizeof(buffer));
    EXPECT_EQ(len, 3);
    EXPECT_STREQ(buffer, "/?=");

    // Test with lowercase hex
    len = uriDecode("%2f%3f%3d", buffer, sizeof(buffer));
    EXPECT_EQ(len, 3);
    EXPECT_STREQ(buffer, "/?=");
}

TEST_F(StringsTest, UriDecode_BufferBoundary) {
    char buffer[20];  // Plenty of space
    size_t len = uriDecode("test%20value", buffer, sizeof(buffer));

    // "test value" = 10 characters
    EXPECT_EQ(len, 10);
    EXPECT_STREQ(buffer, "test value");
}

TEST_F(StringsTest, UriDecode_EmptyString) {
    char buffer[10];
    size_t len = uriDecode("", buffer, sizeof(buffer));
    EXPECT_EQ(len, 0);
    EXPECT_STREQ(buffer, "");
}

TEST_F(StringsTest, UriDecode_OnlyPercentEncoding) {
    char buffer[10];  // Need space for result + null terminator
    size_t len = uriDecode("%20%21%22", buffer, sizeof(buffer));
    EXPECT_EQ(len, 3);
    EXPECT_EQ(buffer[0], ' ');   // %20 = space
    EXPECT_EQ(buffer[1], '!');   // %21 = !
    EXPECT_EQ(buffer[2], '"');   // %22 = "
    EXPECT_EQ(buffer[3], '\0');  // Null terminator
}

TEST_F(StringsTest, UriDecode_MixedContent) {
    char buffer[100];
    size_t len = uriDecode("name=John%20Doe&age=30", buffer, sizeof(buffer));
    // "name=John Doe&age=30" = 20 chars (not counting null terminator)
    EXPECT_EQ(len, 20);
    EXPECT_STREQ(buffer, "name=John Doe&age=30");
}

TEST_F(StringsTest, UriDecode_NullTerminatorAlwaysAdded) {
    char buffer[100] = {'\xFF'};  // Fill with marker
    uriDecode("test", buffer, sizeof(buffer));

    // Verify null terminator is added at position equal to length
    EXPECT_EQ(buffer[4], '\0');
}

TEST_F(StringsTest, UriDecode_BufferSizeSmall) {
    char buffer[3];  // Room for 2 chars + null terminator
    size_t len = uriDecode("hello", buffer, sizeof(buffer));
    // With fixed implementation: max_chars = length - 1 = 2
    // So it writes: h, e (then null terminator at index 2)
    EXPECT_EQ(len, 2);
    EXPECT_EQ(buffer[0], 'h');
    EXPECT_EQ(buffer[1], 'e');
    EXPECT_EQ(buffer[2], '\0');
}

// ============================================================================
// Integration and edge case tests
// ============================================================================

TEST_F(StringsTest, AtolVariousBases) {
    // Test all common bases
    EXPECT_EQ(atol("100", 2), 4);    // Binary
    EXPECT_EQ(atol("100", 8), 64);   // Octal
    EXPECT_EQ(atol("100", 10), 100); // Decimal
    EXPECT_EQ(atol("100", 16), 256); // Hex
}

TEST_F(StringsTest, AllIntegerConversionConsistency) {
    // All should handle "42" the same way
    EXPECT_EQ(atol("42"), 42);
    EXPECT_EQ(atou8("42"), 42);
    EXPECT_EQ(atou16("42"), 42);
    EXPECT_EQ(atou32("42"), 42);
    EXPECT_EQ(atou64("42"), 42);
}

TEST_F(StringsTest, StringViewFromLiteral) {
    // Test that string_view works with string literals
    std::string_view view = "12345";
    EXPECT_EQ(atol(view), 12345);
    EXPECT_EQ(atou8(view), 12345 % 256);
}

TEST_F(StringsTest, StringViewFromString) {
    // Test that string_view works with std::string
    std::string str = "9876";
    std::string_view view(str);
    EXPECT_EQ(atol(view), 9876);
}
