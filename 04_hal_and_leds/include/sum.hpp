#pragma once
#include "i_sum.hpp"

class Sum : public ISum
{
public:
    Sum() = default;

    int add(int a, int b) override;
    int add_constrained(int a, int b) override;
    esp_err_t add_constrained_err(int a, int b, int &result) override;
};