#pragma once

class ISum
{
public:
    virtual ~ISum() = default;
    virtual int add(int a, int b) = 0;
    virtual int add_constrained(int a, int b) = 0;
};