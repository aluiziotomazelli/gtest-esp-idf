// gpio_hal.hpp
#pragma once

#include "i_gpio_hal.hpp"

class GpioHal : public IGpioHal
{
public:
    GpioHal() = default;
    esp_err_t pin_set_direction(gpio_num_t gpio_num, gpio_mode_t mode) override
    {
        return gpio_set_direction(gpio_num, mode);
    }
    esp_err_t pin_set_level(gpio_num_t gpio_num, uint32_t level) override { return gpio_set_level(gpio_num, level); }
};