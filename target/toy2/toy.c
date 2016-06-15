#include <stdio.h>
#include <unistd.h>
#include "toy.h"

int
zero(void)
{
    return 0;
}

int
add_one(int x)
{
    return x+1;
}

int
crashme(int x)
{
    int *y = NULL;
    if (x > 33)
        return *y;
    else
        return x+2;
}
