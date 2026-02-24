#pragma once

#include "esp_err.h"

class ISum
{
public:
    virtual ~ISum() = default;
    virtual int add(int a, int b) = 0;
    virtual int add_constrained(int a, int b) = 0;
    virtual esp_err_t add_constrained_err(int a, int b, int &result) = 0;
};