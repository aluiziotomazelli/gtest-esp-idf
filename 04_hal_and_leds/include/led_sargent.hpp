// led_sargent.hpp
#pragma once

#include "gpio_hal.hpp"
#include "i_led_sargent.hpp"

class LedSargent : public ILedSargent
{
public:
    LedSargent(GpioHal &gpio_hal, gpio_num_t green, gpio_num_t red);

    esp_err_t green() override;
    esp_err_t red() override;
    esp_err_t off() override;

private:
    GpioHal &gpio_hal_;
    gpio_num_t green_;
    gpio_num_t red_;
};