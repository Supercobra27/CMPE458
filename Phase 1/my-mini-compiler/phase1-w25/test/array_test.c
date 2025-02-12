#include <stdio.h>
#include "../include/dynamic_array.h"

struct my_struct
{
    const char name[100];
    int age;
};

int main()
{
    Array *a = array_new(10, sizeof(struct my_struct));
    struct my_struct s1 = {"John Doe", 25};
    struct my_struct s2 = {"Joe Schmoe", 33};
    array_push(a, (Element *)&s1);
    array_push(a, (Element *)&s2);
    struct my_struct *ps1 = array_get(a, 0);
    struct my_struct *ps2 = array_get(a, 1);
    printf("Name: %s, Age: %d\n", ps1->name, ps1->age);
    printf("Name: %s, Age: %d\n", ps2->name, ps2->age);
    array_free(a);

    a = array_new(10, sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        array_push(a, (Element *)&i);
    }

    for (int i = 0; i < 10; i++)
    {
        int *pi = array_get(a, i);
        printf("%d\n", *pi);
    }
    return 0;
}