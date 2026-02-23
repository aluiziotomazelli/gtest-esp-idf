#include <stdio.h>

#include "esp_log.h"

#include "sum.hpp"

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Entering app_main");
    Sum sum;

    ESP_LOGI(TAG, "Calling add");
    int result = sum.add(1, 2);
    int constrained_result = sum.add_constrained(1, 2);

    printf("1 + 2 = %d\n", result);
    printf("1 + 2 = %d\n", constrained_result);

    ESP_LOGI(TAG, "Calling add_constrained_err");

    int constrained_err_result = 0;

    esp_err_t err = sum.add_constrained_err(4, 5, constrained_err_result);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "4 + 5 = %d", constrained_err_result);
    }

    err = sum.add_constrained_err(6, 6, constrained_err_result);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "6 + 6 = %d", constrained_err_result);
    }

    err = sum.add_constrained_err(1, 11, constrained_err_result);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "1 + 11 = %d", constrained_err_result);
    }
}