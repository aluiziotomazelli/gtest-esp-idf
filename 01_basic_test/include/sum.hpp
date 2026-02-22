#pragma once
#include "i_sum.hpp"

class Sum : public ISum
{
public:
    int add(int a, int b) override;
    int add_constrained(int a, int b) override;
};