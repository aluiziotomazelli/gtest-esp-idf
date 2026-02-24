#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "i_sum.hpp"
#include "sum_boss.hpp"

/**
 * @brief Mock class for the ISum interface.
 *
 * This mock allows tests to replace the real Sum implementation with a controlled
 * substitute. Using Google Mock, we can define expectations on method calls,
 * specify return values, and even modify output parameters.
 *
 * The mock is essential for testing higher-level components (like SumBoss)
 * in isolation — we can verify that they interact with ISum correctly without
 * bringing in the actual arithmetic logic.
 */
class MockSum : public ISum
{
public:
    // Mock each virtual method from ISum.
    // MOCK_METHOD macro syntax: (return_type, method_name, (parameters), (override))

    MOCK_METHOD(int, add, (int a, int b), (override));
    MOCK_METHOD(int, add_constrained, (int a, int b), (override));
    MOCK_METHOD(esp_err_t, add_constrained_err, (int a, int b, int &result), (override));
};

// Bring commonly used GMock matchers and actions into scope.
// This makes test code more readable (e.g., using _ instead of testing::_).
using ::testing::_;             // Wildcard matcher — matches any value
using ::testing::DoAll;         // Executes multiple actions in sequence
using ::testing::Return;        // Specifies a return value for a mocked method
using ::testing::SetArgReferee; // Writes a value into a reference argument

/**
 * @test Verifies that SumBoss correctly delegates to ISum and propagates the result.
 *
 * This test does NOT check the arithmetic logic itself — that is the responsibility
 * of the Sum class tests. Instead, it focuses on the orchestration layer:
 *
 * 1. SumBoss should call ISum::add_constrained_err with the exact arguments it received.
 * 2. It should forward the result (via reference) back to the caller.
 * 3. It should propagate the return value (esp_err_t) unchanged.
 *
 * The test uses a mock to isolate SumBoss from the real implementation.
 * This way, we can verify the "glue" logic independently of the arithmetic.
 */
TEST(SumBossTest, DelegatesToSumAndReturnsResult)
{
    // Create a mock instance of the ISum interface.
    // The mock is lightweight and lets us control exactly what SumBoss sees.
    MockSum mock;

    // Inject the mock into SumBoss — SumBoss has no idea it's talking to a mock.
    SumBoss boss(mock);

    // Set up expectations: we expect SumBoss to call add_constrained_err(3, 4, _)
    // with any value for the result parameter (the _ wildcard means "don't care").
    int expected_result = 7;
    EXPECT_CALL(mock, add_constrained_err(3, 4, _))
        .WillOnce(DoAll(
            SetArgReferee<2>(expected_result), // Write 7 into the result argument
            Return(ESP_OK)                     // Return ESP_OK to the caller
            ));

    // Call the method under test.
    int result;
    esp_err_t err = boss.compute(3, 4, result);

    // Verify the outcomes:
    // - The error code returned by SumBoss matches what the mock returned.
    // - The result value matches what the mock wrote into the reference.
    EXPECT_EQ(ESP_OK, err);
    EXPECT_EQ(expected_result, result);

    // If both checks pass, it means SumBoss correctly delegated the call
    // and propagated both the output parameter and the return value.
}

/**
 * @test Verifies that SumBoss correctly propagates an error from the underlying ISum.
 *
 * This test complements the happy path test by checking the error propagation:
 * - When ISum returns an error code, SumBoss must return the same code.
 * - The result parameter may be left untouched (the test doesn't rely on it).
 *
 * This is crucial for robust error handling: the orchestrator should not
 * swallow or alter error codes — it must pass them through transparently.
 */
TEST(SumBossTest, PropagatesErrorFromSum)
{
    // Create a mock instance and inject it into SumBoss.
    MockSum mock;
    SumBoss boss(mock);

    // Set up the expectation: add_constrained_err(11,5) will be called,
    // and it will return ESP_INVALID_ARG (our function dont accept number higher than 10).
    // Note: we don't care about the result parameter here (wildcard _).
    EXPECT_CALL(mock, add_constrained_err(11, 5, _)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    // Call the method under test with inputs that should trigger an error.
    int result = 0xDEAD; // Initialize with a recognizable sentinel value.
    esp_err_t err = boss.compute(11, 5, result);

    // Verify that the error code from ISum made it through SumBoss unchanged.
    EXPECT_EQ(ESP_ERR_INVALID_ARG, err);

    // Note: We don't check 'result' here — when an error occurs,
    // its value is unspecified. The test only cares about the error code.
}
