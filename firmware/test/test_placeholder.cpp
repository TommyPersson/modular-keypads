#include <gtest/gtest.h>

TEST(DummyTest, ShouldPass)
{
    EXPECT_EQ(1, 1);
}

void Sub1(int n)
{
    EXPECT_EQ(n, 1);
}

TEST(FooTest, Bar)
{
    {
        SCOPED_TRACE("A"); // This trace point will be included in
        // every failure in this scope.
        Sub1(1);
    }
    // Now it won't.
    Sub1(1);
}

TEST(SkipTest, DoesSkip)
{
    GTEST_SKIP() << "Skipping single test";
    EXPECT_EQ(0, 1); // Won't fail; it won't be executed
}
