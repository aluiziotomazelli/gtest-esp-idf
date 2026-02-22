#include "sum.hpp"

int Sum::add(int a, int b)
{
    return a + b;
}

int Sum::add_constrained(int a, int b)
{
    if (a < 0 || a > 10 || b < 0 || b > 10) {
        return -1;
    }

    int result = a + b;

    if (result > 10) {
        return -1;
    }

    return result;
}
