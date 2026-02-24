#pragma once

#include "i_led_sargent.hpp"
#include "i_sum.hpp"

class SumBoss
{
public:
    SumBoss(ISum &sum, ILedSargent &led_sargent);
    ~SumBoss() = default;

    esp_err_t compute(int a, int b, int &result);

private:
    ISum &sum_;
    ILedSargent &led_sargent_;
};