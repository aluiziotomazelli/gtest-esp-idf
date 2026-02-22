#include "gtest/gtest.h"

#include "sum.hpp"

/**
 * @brief Basic verification test (Smoke Test).
 * * This test serves as a baseline to validate that the GoogleTest framework
 * is correctly integrated into the ESP-IDF environment. If this test passes,
 * it confirms that the build infrastructure and linker flags are operational.
 */
TEST(TestSum, GTestSmokeTest)
{
    /* Simple assertion to verify gtest's core functionality */
    EXPECT_TRUE(true);
}

/**
 * @brief Unit tests for the Sum class.
 * This suite validates the basic arithmetic logic of the add function
 * to ensure the component performs correctly under standard conditions.
 */

/**
 * @test Verifies that the add function returns the correct sum of two integers.
 */
TEST(TestSum, BasicAddition)
{
    Sum calc;

    // Check positive integer addition
    int result = calc.add(2, 2);

    // Compares 'result' with 4; if they differ, gtest records a non-fatal failure.
    EXPECT_EQ(result, 4);

    // Check addition involving zero
    result = calc.add(0, 5);

    // Expected result
    EXPECT_EQ(result, 5);
}

/**
 * @test Verifies that the add function correctly handles negative integers.
 */
TEST(TestSum, NegativeAddition)
{
    Sum calc;

    // Check addition of two negative numbers
    EXPECT_EQ(calc.add(-1, -1), -2);

    // Check addition of positive and negative numbers
    EXPECT_EQ(calc.add(-5, 10), 5);
}
