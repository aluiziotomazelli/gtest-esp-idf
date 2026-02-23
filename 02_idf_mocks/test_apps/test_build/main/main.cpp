#include <stdio.h>

#include "esp_log.h"

#include "sum.hpp"

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Entering app_main");

    // Intacialize the Sum class object
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
}