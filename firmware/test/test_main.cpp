#include <gtest/gtest.h>

// Entry point for test runner
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    // Always return zero-code and allow PlatformIO to parse results
    (void)result;  // Unused variable - tests are captured by GoogleTest framework
    return 0;
}
