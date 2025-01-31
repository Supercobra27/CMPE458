#include <stdio.h>
#include "../include/dynamic_array.h"
#include <stddef.h>
#include <stdlib.h>

struct my_struct
{
    const char name[100];
    int age;
};

/**
 * A buffer structure containing count entries of the given size.
 * The first entry of the buffer is stored in the location of the buf pointer.
 * All subsequent entries are stored in memory directly after the first entry.
 */
typedef struct
{
    size_t size;
    int count;
    void *buf;
} buffer_t;

/** Allocate a new buffer_t with "count" entries of "size" size. */
buffer_t *buffer_new(size_t size, int count)
{
    buffer_t *p = malloc(offsetof(buffer_t, buf) + count * size);
    if (p)
    {
        p->size = size;
        p->count = count;
    }
    return p;
}

/** Get a pointer to the i'th entry. */
void *buffer_get(buffer_t *t, int i)
{
    return (void *)((char *)&t->buf + i * t->size);
}

int main()
{
    buffer_t *b = buffer_new(sizeof(int), 10);

    for (int i = 0; i < 10; i++)
    {
        int *pi = buffer_get(b, i);
        *pi = i;
    }
    for (int i = 0; i < 10; i++)
    {
        int *pi = buffer_get(b, i);
        printf("%d\n", *pi);
    }
    printf("%d\n", (int)b->buf);
    // Array *a = array_new(10, sizeof(struct my_struct));
    // struct my_struct s1 = {"John Doe", 25};
    // struct my_struct s2 = {"Joe Schmoe", 33};
    // array_push(a, (Element *)&s1);
    // array_push(a, (Element *)&s2);
    // struct my_struct *ps1 = array_get(a, 0);
    // struct my_struct *ps2 = array_get(a, 1);
    // printf("Name: %s, Age: %d\n", ps1->name, ps1->age);
    // printf("Name: %s, Age: %d\n", ps2->name, ps2->age);
    // array_free(a);

    // a = array_new(10, sizeof(int));
    // for (int i = 0; i < 10; i++)
    // {
    //     array_push(a, (Element *)&i);
    // }

    // for (int i = 0; i < 10; i++)
    // {
    //     int *pi = array_get(a, i);
    //     printf("%d\n", *pi);
    // }
    return 0;
}