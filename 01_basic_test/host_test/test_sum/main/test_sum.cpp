#include "gtest/gtest.h"

#include "sum.hpp"

// ======================================================================
// Smoke Test
// ======================================================================
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

// ======================================================================
// Basic tests for the add(int a, int b) function
// ======================================================================

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

// ======================================================================
// Basic tests for the add_constrained(int a, int b) function
// ======================================================================

/**
 * @test Happy Path: Verifies that valid inputs within bounds return the correct sum.
 */
TEST(TestSum, AddConstrained_HappyPath)
{
    Sum calc;

    // Test a standard valid case
    int result = calc.add_constrained(3, 4);
    EXPECT_EQ(result, 7);
}

/**
 * @test Edge Cases: Verifies behavior at the exact boundaries of the 0-10 range.
 */
TEST(TestSum, AddConstrained_EdgeCases)
{
    Sum calc;

    // Minimum boundary: 0 + 0 should be 0
    EXPECT_EQ(calc.add_constrained(0, 0), 0);

    // Maximum boundary: 5 + 5 should be 10 (the upper limit)
    EXPECT_EQ(calc.add_constrained(5, 5), 10);

    // Maximum single input: 10 + 0 should be 10
    EXPECT_EQ(calc.add_constrained(10, 0), 10);
}

/**
 * @test Out of Range: Verifies that inputs or results exceeding limits return -1.
 */
TEST(TestSum, AddConstrained_OutOfRange)
{
    Sum calc;

    // Input above the limit of 10
    EXPECT_EQ(calc.add_constrained(11, 0), -1);

    // Negative input below 0
    EXPECT_EQ(calc.add_constrained(-1, 5), -1);

    // Result exceeds 10 despite valid individual inputs (6 + 5 = 11)
    EXPECT_EQ(calc.add_constrained(6, 5), -1);
}