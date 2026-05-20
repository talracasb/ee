#include <stdio.h>
#include <math.h>
#include "alloc.h"

int main()
{
    char *ptrs[65536];

    for (unsigned long i = 0; i < 65536; i++)
    {
        ptrs[i] = malloc(16);
    }
}
