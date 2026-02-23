#include "gtest/gtest.h"

#include "sum.hpp"

/**
 * Struct to store test parameters for the parameterized test class.
 */
struct SumParams
{
    int a;
    int b;
    int result;
    esp_err_t error;
};

/**
 * @brief A parameterized test class for the Sum component.
 *
 * This class allows writing parameterized tests for the Sum component using
 * the ::testing::TestWithParam<> class from GoogleTest. Each test case
 * is specified as a struct containing the input parameters and the expected
 * result.
 */
class SumParamTest : public ::testing::TestWithParam<SumParams>
{
protected:
    Sum calc;
};

/**
 * @test Verifies that the add_constrained_err(int a, int b, int &result) function
 * correctly handles valid input values.
 *
 * This test runs multiple times with different test parameters.
 * The parameters are: a, b, result, error.
 *
 * The test verifies that the add_constrained_err function returns the expected
 * result and error code when the input values are valid.
 */
TEST_P(SumParamTest, AddConstrainedErr)
{
    const auto &params = GetParam(); // Colects the test parameters for each run
    int result = 0xDEADBEEF;         // Dummy value

    // Run funcion and collect result
    esp_err_t err = calc.add_constrained_err(params.a, params.b, result);

    // Verify error
    EXPECT_EQ(err, params.error);

    // Verify result
    EXPECT_EQ(result, params.result);
}

/**
 * @test Verifies that the add_constrained_err(int a, int b, int &result) function
 * correctly handles invalid input values.
 *
 * This test runs multiple times with different test parameters.
 * The parameters are: a, b, result, error.
 *
 * The test verifies that the add_constrained_err function returns the expected
 * result and error code when the input values are invalid.
 */
INSTANTIATE_TEST_SUITE_P(
    SumParamTest,
    SumParamTest,
    ::testing::Values(
        SumParams{3, 4, 7, ESP_OK},                // (Happy path): a = 3, b = 4, result = 7, error = ESP_OK
        SumParams{4, 5, 9, ESP_OK},                // (Happy path): a = 4, b = 5, result = 9, error = ESP_OK
        SumParams{0, 0, 0, ESP_OK},                // (Edge case): a = 0, b = 0, result = 0, error = ESP_OK
        SumParams{5, 5, 10, ESP_OK},               // (Edge case): a = 5, b = 5, result = 10, error = ESP_OK
        SumParams{10, 0, 10, ESP_OK},              // (Edge case): a = 10, b = 0, result = 10, error = ESP_OK
        SumParams{11, 0, -1, ESP_ERR_INVALID_ARG}, // (Error case): a = 11, b = 0, result = _, error = INVALID_ARG
        SumParams{-1, 5, -1, ESP_ERR_INVALID_ARG}, // (Error case): a = -1, b = 5, result = _, error = INVALID_ARG
        SumParams{6, 5, -1, ESP_FAIL}              // (Error case): a = 6, b = 5, result = _, error = ESP_FAIL

        ));