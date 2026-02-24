#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "gpio_hal.hpp"
#include "led_sargent.hpp"
#include "sum.hpp"
#include "sum_boss.hpp"

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Entering app_main");

    // Instace Sum class object
    Sum sum;

    ESP_LOGI(TAG, "Calling add");

    // Simple add function, returns int
    int result = sum.add(1, 2);
    int constrained_result = sum.add_constrained(1, 2);

    printf("1 + 2 = %d\n", result);
    printf("1 + 2 = %d\n", constrained_result);

    ESP_LOGI(TAG, "Calling add_constrained_err");

    // Declare constrained_err_result variable
    int constrained_err_result = 0;

    // Constrained add function, returns esp_err_t and the result via reference
    esp_err_t err = sum.add_constrained_err(4, 5, constrained_err_result); // 4 + 5 will return ESP_OK
    if (err == ESP_OK) {
        // This line will be printed in log
        ESP_LOGI(TAG, "4 + 5 = %d", constrained_err_result);
    }

    err = sum.add_constrained_err(6, 6, constrained_err_result); // 6 + 6 will NOT return ESP_OK
    if (err == ESP_OK) {
        // This line will NOT be printed in log but the class itself will log the error
        ESP_LOGI(TAG, "6 + 6 = %d", constrained_err_result);
    }

    err = sum.add_constrained_err(1, 11, constrained_err_result);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "1 + 11 = %d", constrained_err_result);
    }

    ESP_LOGI(TAG, "Calling SumBoss");

    // Instance SumBoss class object with Sum class object (sum) as dependency injection
    SumBoss sum_boss(sum);

    int result_boss = 0;
    err = sum_boss.compute(1, 2, result_boss); // 1 + 2 will return ESP_OK
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "1 + 2 = %d", result_boss);
    }

    err = sum_boss.compute(0, 11, result_boss); // 0 + 11 will NOT return ESP_OK
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "1 + 11 = %d", result_boss);
    }

    err = sum_boss.compute(8, 8, result_boss); // 8 + 8 will NOT return ESP_OK
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "11 + 0 = %d", result_boss);
    }

    GpioHal gpio_hal;
    LedSargent led_sargent(gpio_hal, GPIO_NUM_4, GPIO_NUM_5);
    led_sargent.off();
    led_sargent.green();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_sargent.red();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_sargent.off();
    vTaskDelay(pdMS_TO_TICKS(500));
}