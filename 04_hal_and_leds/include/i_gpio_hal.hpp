#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

class IGpioHal
{
public:
    virtual ~IGpioHal() = default;
    virtual esp_err_t pin_set_direction(gpio_num_t pin, gpio_mode_t mode) = 0;
    virtual esp_err_t pin_set_level(gpio_num_t pin, uint32_t level) = 0;
};