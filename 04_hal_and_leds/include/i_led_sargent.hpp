// i_led_sargent.hpp
#pragma once

#include "esp_err.h"

class ILedSargent
{
public:
    virtual ~ILedSargent() = default;
    virtual esp_err_t green() = 0;
    virtual esp_err_t red() = 0;
    virtual esp_err_t off() = 0;
};