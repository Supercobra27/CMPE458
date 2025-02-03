#include <stdio.h>
#include <ctype.h>

int main()
{
    int count = 0;
    for (int c = 0; c < 128; c++)
    {
        if (isprint(c))
        {
            count++;
            printf("%c, %d\n", c, count);
        }
    }
    printf("Number of printable characters: %d\n", count);
}