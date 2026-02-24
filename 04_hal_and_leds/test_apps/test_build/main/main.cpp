#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "gpio_hal.hpp"
#include "led_sargent.hpp"
#include "sum.hpp"
#include "sum_boss.hpp"

// GPIO pin assignments for the LEDs
#define GREEN_LED_PIN GPIO_NUM_4
#define RED_LED_PIN GPIO_NUM_5

// How long each result stays visible on the LEDs before moving to the next
#define RESULT_DELAY_MS 2000

// Short pause between turning a LED off and starting the next calculation,
// just to make sure the LED is fully off before the next one lights up
#define INTER_DELAY_MS 200

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "--- Tutorial: GTest with ESP-IDF ---");

    // ---------------------------------------------------------------
    // Section 1: Sum — basic arithmetic
    // The simplest form: add() returns an int directly.
    // ---------------------------------------------------------------
    ESP_LOGI(TAG, "[1] Sum::add — basic addition");

    Sum sum;

    int result = sum.add(1, 2);
    printf("  1 + 2 = %d\n", result);

    result = sum.add_constrained(3, 4);
    printf("  3 + 4 (constrained) = %d\n", result);

    vTaskDelay(pdMS_TO_TICKS(RESULT_DELAY_MS));

    // ---------------------------------------------------------------
    // Section 2: Sum — constrained addition with esp_err_t
    // add_constrained_err() returns the result via reference and uses
    // esp_err_t to signal success or failure. The function itself logs
    // the error internally when something goes wrong.
    // ---------------------------------------------------------------
    ESP_LOGI(TAG, "[2] Sum::add_constrained_err — error codes via esp_err_t");

    int constrained_result = 0;

    // Valid input: 4 + 5 = 9, within bounds
    esp_err_t err = sum.add_constrained_err(4, 5, constrained_result);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "  4 + 5 = %d", constrained_result);
    }

    // Invalid result: 6 + 6 = 12, exceeds the limit of 10 — returns ESP_FAIL
    err = sum.add_constrained_err(6, 6, constrained_result);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "  6 + 6 => %s (result exceeds limit)", esp_err_to_name(err));
    }

    // Invalid input: 11 is out of the 0-10 range — returns ESP_ERR_INVALID_ARG
    err = sum.add_constrained_err(1, 11, constrained_result);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "  1 + 11 => %s (input out of range)", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(RESULT_DELAY_MS));

    // ---------------------------------------------------------------
    // Section 3: LedSargent — hardware abstraction in action
    // GpioHal wraps the ESP-IDF GPIO driver behind IGpioHal.
    // LedSargent uses IGpioHal to control the LEDs without knowing
    // which GPIO library is underneath — the same interface that
    // MockGpioHal implements in the host tests.
    // ---------------------------------------------------------------
    ESP_LOGI(TAG, "[3] LedSargent — testing the LEDs");

    GpioHal gpio_hal;
    LedSargent led_sargent(gpio_hal, GREEN_LED_PIN, RED_LED_PIN);

    led_sargent.off();

    ESP_LOGI(TAG, "  Green LED on");
    led_sargent.green();
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI(TAG, "  Red LED on");
    led_sargent.red();
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI(TAG, "  Both off");
    led_sargent.off();

    vTaskDelay(pdMS_TO_TICKS(RESULT_DELAY_MS));

    // ---------------------------------------------------------------
    // Section 4: SumBoss — the orchestrator
    // SumBoss receives Sum and LedSargent via dependency injection.
    // It delegates the calculation to Sum and lights the correct LED
    // based on the result. This is the same wiring that the host tests
    // validate — except here it runs on real hardware.
    // ---------------------------------------------------------------
    ESP_LOGI(TAG, "[4] SumBoss — orchestrating Sum and LedSargent");

    SumBoss sum_boss(sum, led_sargent);

    struct TestCase
    {
        int a;
        int b;
    };

    TestCase cases[] = {{1, 2}, {6, 6}, {5, 5}, {-1, 5}, {11, 0}};
    int num_cases = sizeof(cases) / sizeof(cases[0]);

    while (true) {
        for (int i = 0; i < num_cases; i++) {
            int a = cases[i].a;
            int b = cases[i].b;
            int res = 0;

            ESP_LOGI(TAG, "  compute(%d, %d)", a, b);
            err = sum_boss.compute(a, b, res);

            if (err == ESP_OK) {
                ESP_LOGI(TAG, "  => %d + %d = %d [green]", a, b, res);
            }
            else {
                ESP_LOGE(TAG, "  => %d + %d failed: %s [red]", a, b, esp_err_to_name(err));
            }

            // Hold the LED long enough to be visible, then clear before next case
            vTaskDelay(pdMS_TO_TICKS(RESULT_DELAY_MS));
            led_sargent.off();
            vTaskDelay(pdMS_TO_TICKS(INTER_DELAY_MS));
        }
    }
}