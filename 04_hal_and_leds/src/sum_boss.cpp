#include "sum_boss.hpp"

SumBoss::SumBoss(ISum &sum, ILedSargent &led_sargent)
    : sum_(sum)
    , led_sargent_(led_sargent)
{
}

esp_err_t SumBoss::compute(int a, int b, int &result)
{
    esp_err_t ret = sum_.add_constrained_err(a, b, result);
    if (ret == ESP_OK) {            // no error
        ret = led_sargent_.green(); // check if green led works
        if (ret != ESP_OK) {        // green led does not work
            return ret;             // return the error
        }
    }
    else {                  // if the sum failed
        led_sargent_.red(); // the sum error is already in ret
    }
    return ret;
}