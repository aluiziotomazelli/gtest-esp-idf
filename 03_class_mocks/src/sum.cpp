
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

#include "sum.hpp"

static const char *TAG = "SUM";

int Sum::add(int a, int b)
{
    return a + b;
}

int Sum::add_constrained(int a, int b)
{
    if (a < 0 || a > 10 || b < 0 || b > 10) {
        return -1;
    }

    int result = a + b;

    if (result > 10) {
        return -1;
    }

    return result;
}

esp_err_t Sum::add_constrained_err(int a, int b, int &result)
{
    result = -1; // Invalid result on initialization
    if (a < 0 || a > 10 || b < 0 || b > 10) {
        esp_err_t err = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "Invalid params: a = %d, b = %d, error = %s", a, b, esp_err_to_name(err));
        return err;
    }

    int sum = a + b;

    if (sum > 10) {
        esp_err_t err = ESP_FAIL;
        ESP_LOGE(TAG, "Invalid result: sum = %d, error=%s", sum, esp_err_to_name(err));
        return err;
    }

    result = sum;
    return ESP_OK;
}