#include <ctype.h>
#include <string.h>

#include "../include/operators.h"

int operator_index(const char *const s)
{
    for (unsigned i = 0; i < (sizeof(reduced_operators) / sizeof(reduced_operators[0])); i++)
        if (strncmp(reduced_operators[i], s, strlen(reduced_operators[i])) == 0)
            return i;
    return -1;
}

