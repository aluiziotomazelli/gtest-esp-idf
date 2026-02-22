#include "sum.hpp"
#include <stdio.h>

extern "C" void app_main(void)
{
    Sum sum;
    int result = sum.add(1, 2);
    int constrained_result = sum.add_constrained(1, 2);

    printf("1 + 2 = %d\n", result);
    printf("1 + 2 = %d\n", constrained_result);
}