#include "sum_boss.hpp"

SumBoss::SumBoss(ISum &sum)
    : sum_(sum)
{
}

esp_err_t SumBoss::compute(int a, int b, int &result)
{
    esp_err_t ret = sum_.add_constrained_err(a, b, result);
    return ret;
}