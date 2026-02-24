// led_sargent.cpp

#include "led_sargent.hpp"
#include "gpio_hal.hpp"

LedSargent::LedSargent(GpioHal &gpio_hal, gpio_num_t green, gpio_num_t red)
    : gpio_hal(gpio_hal)
    , green(green)
    , red(red)
{
    gpio_hal.pin_set_direction(green, GPIO_MODE_OUTPUT);
    gpio_hal.pin_set_direction(red, GPIO_MODE_OUTPUT);
}

esp_err_t LedSargent::green()
{
    return gpio_hal.pin_set_level(green, 1);
}
esp_err_t LedSargent::red()
{
    return gpio_hal.pin_set_level(red, 1);
}
esp_err_t LedSargent::off()
{
    esp_err_t ret;
    ret = gpio_hal.pin_set_level(green, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = gpio_hal.pin_set_level(red, 0);
    return ret;
}
