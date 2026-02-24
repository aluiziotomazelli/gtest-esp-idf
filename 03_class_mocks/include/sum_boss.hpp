#pragma once

#include "i_sum.hpp"

class SumBoss
{
public:
    SumBoss(ISum &sum);
    ~SumBoss() = default;

    esp_err_t compute(int a, int b, int &result);

private:
    ISum &sum_;
};