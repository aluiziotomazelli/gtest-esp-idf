#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "i_led_sargent.hpp"
#include "i_sum.hpp"
#include "sum.hpp"
#include "sum_boss.hpp"

// Mock class for ISum
class MockSum : public ISum
{
public:
    // Mock each virtual method from ISum.
    // MOCK_METHOD macro syntax: (return_type, method_name, (parameters), (override))

    MOCK_METHOD(int, add, (int a, int b), (override));
    MOCK_METHOD(int, add_constrained, (int a, int b), (override));
    MOCK_METHOD(esp_err_t, add_constrained_err, (int a, int b, int &result), (override));
};

// Mock class for ILedSargent
class MockLedSargent : public ILedSargent
{
public:
    MOCK_METHOD(esp_err_t, green, (), (override));
    MOCK_METHOD(esp_err_t, red, (), (override));
    MOCK_METHOD(esp_err_t, off, (), (override));
};

// Bring commonly used GMock matchers and actions into scope.
// This makes test code more readable (e.g., using _ instead of testing::_).
using ::testing::_;             // Wildcard matcher — matches any value
using ::testing::DoAll;         // Executes multiple actions in sequence
using ::testing::NiceMock;      // Creates a "nice" mock
using ::testing::Return;        // Specifies a return value for a mocked method
using ::testing::SetArgReferee; // Writes a value into a reference argument

TEST(SumBossTest, CallsGreenOnSuccess)
{
    NiceMock<MockSum> mock_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(mock_sum, mock_led);

    // Expects add_constrained_err(3, 4) to be called once.
    // SetArgReferee<2>(7) writes 7 into the result reference argument,
    // simulating what the real Sum would do.
    EXPECT_CALL(mock_sum, add_constrained_err(3, 4, _)).WillOnce(DoAll(SetArgReferee<2>(7), Return(ESP_OK)));

    // green() must be called once and returns ESP_OK
    EXPECT_CALL(mock_led, green()).WillOnce(Return(ESP_OK));

    // red() must NOT be called
    EXPECT_CALL(mock_led, red()).Times(0);

    int result;
    esp_err_t err = boss.compute(3, 4, result);

    EXPECT_EQ(ESP_OK, err);
    EXPECT_EQ(7, result); // result must match what the mock wrote via SetArgReferee
}

TEST(SumBossTest, CallsRedOnError)
{
    NiceMock<MockSum> mock_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(mock_sum, mock_led);

    // We don't care about the result value on error, so _ ignores it
    EXPECT_CALL(mock_sum, add_constrained_err(6, 6, _)).WillOnce(Return(ESP_FAIL));
    EXPECT_CALL(mock_led, green()).Times(0);
    EXPECT_CALL(mock_led, red()).WillOnce(Return(ESP_OK));

    int result;
    esp_err_t err = boss.compute(6, 6, result);

    EXPECT_EQ(ESP_FAIL, err); // sum error must propagate, led error is ignored
}

TEST(SumBossTest, ReturnsLedErrorIfGreenFails)
{
    NiceMock<MockSum> mock_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(mock_sum, mock_led);

    EXPECT_CALL(mock_sum, add_constrained_err(3, 4, _)).WillOnce(DoAll(SetArgReferee<2>(7), Return(ESP_OK)));

    // green() fails — boss must propagate the LED error
    EXPECT_CALL(mock_led, green()).WillOnce(Return(ESP_ERR_INVALID_ARG));
    EXPECT_CALL(mock_led, red()).Times(0);

    int result;
    esp_err_t err = boss.compute(3, 4, result);

    EXPECT_EQ(ESP_ERR_INVALID_ARG, err);
}

TEST(SumBossTest, PropagatesRedIgnoresLedError)
{
    NiceMock<MockSum> mock_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(mock_sum, mock_led);

    EXPECT_CALL(mock_sum, add_constrained_err(6, 6, _)).WillOnce(Return(ESP_FAIL));
    EXPECT_CALL(mock_led, green()).Times(0);

    // red() fails, but boss must still return the sum error, not the LED error
    EXPECT_CALL(mock_led, red()).WillOnce(Return(ESP_ERR_INVALID_ARG));

    int result;
    esp_err_t err = boss.compute(6, 6, result);

    EXPECT_EQ(ESP_FAIL, err);
}

// Integration tests — real Sum, mocked LED
TEST(SumBossRealSumTest, CallsGreenOnSuccess)
{
    Sum real_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(real_sum, mock_led);

    // No mock on sum — the real Sum decides the result
    EXPECT_CALL(mock_led, green()).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_led, red()).Times(0);

    int result;
    esp_err_t err = boss.compute(3, 4, result);

    EXPECT_EQ(ESP_OK, err);
    EXPECT_EQ(7, result);
}

TEST(SumBossRealSumTest, CallsRedOnError)
{
    Sum real_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(real_sum, mock_led);

    EXPECT_CALL(mock_led, green()).Times(0);
    EXPECT_CALL(mock_led, red()).WillOnce(Return(ESP_OK));

    int result;
    esp_err_t err = boss.compute(6, 6, result); // 6 + 6 = 12 > 10, expects ESP_FAIL

    EXPECT_EQ(ESP_FAIL, err);
}

TEST(SumBossRealSumTest, CallsRedWithInvalidArg)
{
    Sum real_sum;
    NiceMock<MockLedSargent> mock_led;
    SumBoss boss(real_sum, mock_led);

    EXPECT_CALL(mock_led, green()).Times(0);
    EXPECT_CALL(mock_led, red()).WillOnce(Return(ESP_ERR_INVALID_ARG));

    int result;
    esp_err_t err = boss.compute(-3, 4, result); // negative input, expects ESP_ERR_INVALID_ARG

    EXPECT_EQ(ESP_ERR_INVALID_ARG, err);
}