#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "i_gpio_hal.hpp"
#include "led_sargent.hpp"

// -------------------------------------------------------------------
// Mock class for the GPIO HAL interface
// -------------------------------------------------------------------
class MockGpioHal : public IGpioHal
{
public:
    MOCK_METHOD(esp_err_t, pin_set_direction, (gpio_num_t, gpio_mode_t), (override));
    MOCK_METHOD(esp_err_t, pin_set_level, (gpio_num_t, uint32_t), (override));
};

using ::testing::Return;

// -------------------------------------------------------------------
// Constructor test — explicit expectations
// -------------------------------------------------------------------
/**
 * @test Verifies that the LedSargent constructor configures both pins
 *       as outputs using the correct GPIO numbers and mode.
 *
 * This test uses a regular mock (not NiceMock) because we want to be
 * strict: any unexpected call would fail the test. The constructor
 * must call pin_set_direction exactly twice, with the expected pins,
 * and nothing else.
 */
TEST(LedSargentTest, ConstructorSetsCorrectPins)
{
    MockGpioHal mock_hal;

    // Expect the two direction calls — order doesn't matter,
    // but both must happen exactly once.
    EXPECT_CALL(mock_hal, pin_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_hal, pin_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT)).WillOnce(Return(ESP_OK));

    LedSargent led(mock_hal, GPIO_NUM_2, GPIO_NUM_4);
    // If the constructor makes any other GPIO calls, the test fails.
}

// -------------------------------------------------------------------
// Method tests — using NiceMock to ignore constructor calls
// -------------------------------------------------------------------
/**
 * @test Verifies that led.green() sets the correct GPIO pin high.
 *
 * Here we use NiceMock<MockGpioHal>. A NiceMock suppresses warnings
 * about "uninteresting calls" — in this case, the two pin_set_direction
 * calls made by the constructor. Without NiceMock, those calls would
 * be reported as unexpected (even though they are part of normal setup).
 *
 * NiceMock lets us focus on the behavior we actually want to test.
 */
TEST(LedSargentTest, GreenSetsCorrectPin)
{
    ::testing::NiceMock<MockGpioHal> mock_hal;

    // Constructor will call pin_set_direction twice — NiceMock ignores them.
    LedSargent led(mock_hal, GPIO_NUM_2, GPIO_NUM_4);

    // Now we set an expectation for the actual call we care about.
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_2, 1)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, led.green());
}

/**
 * @test Verifies that led.red() sets the correct GPIO pin high.
 */
TEST(LedSargentTest, RedSetsCorrectPin)
{
    ::testing::NiceMock<MockGpioHal> mock_hal;
    LedSargent led(mock_hal, GPIO_NUM_2, GPIO_NUM_4);

    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_4, 1)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, led.red());
}

/**
 * @test Verifies that led.off() turns both pins low.
 */
TEST(LedSargentTest, OffSetsCorrect)
{
    ::testing::NiceMock<MockGpioHal> mock_hal;
    LedSargent led(mock_hal, GPIO_NUM_2, GPIO_NUM_4);

    // Both pins must be set low.
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_2, 0)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_4, 0)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, led.off());
}

/**
 * @test Verifies that led.off() stops and returns the error if the first
 *       pin operation fails. The second pin should not be touched.
 */
TEST(LedSargentTest, OffAbortsOnFirstFailure)
{
    ::testing::NiceMock<MockGpioHal> mock_hal;
    LedSargent led(mock_hal, GPIO_NUM_2, GPIO_NUM_4);

    // First call fails
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_2, 0)).WillOnce(Return(ESP_FAIL));

    // Second call must never happen
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_4, 0)).Times(0);

    EXPECT_EQ(ESP_FAIL, led.off());
}

/**
 * @test Verifies that led.off() returns the error if the first pin
 *       succeeds but the second one fails.
 */
TEST(LedSargentTest, OffAbortsOnSecondFailure)
{
    ::testing::NiceMock<MockGpioHal> mock_hal;
    LedSargent led(mock_hal, GPIO_NUM_2, GPIO_NUM_4);

    // First call succeeds
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_2, 0)).WillOnce(Return(ESP_OK));

    // Second call fails — error should propagate
    EXPECT_CALL(mock_hal, pin_set_level(GPIO_NUM_4, 0)).WillOnce(Return(ESP_FAIL));

    EXPECT_EQ(ESP_FAIL, led.off());
}
