// led_sargent.cpp

#include "led_sargent.hpp"

LedSargent::LedSargent(IGpioHal &gpio_hal, gpio_num_t green, gpio_num_t red)
    : gpio_hal_(gpio_hal)
    , green_(green)
    , red_(red)
{
    gpio_hal_.pin_set_direction(green, GPIO_MODE_OUTPUT);
    gpio_hal_.pin_set_direction(red, GPIO_MODE_OUTPUT);
}

esp_err_t LedSargent::green()
{
    return gpio_hal_.pin_set_level(green_, 1);
}
esp_err_t LedSargent::red()
{
    return gpio_hal_.pin_set_level(red_, 1);
}
esp_err_t LedSargent::off()
{
    esp_err_t ret;
    ret = gpio_hal_.pin_set_level(green_, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = gpio_hal_.pin_set_level(red_, 0);
    return ret;
}
