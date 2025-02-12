#include "../include/dynamic_array.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct Array_t
{
    size_t capacity;
    size_t element_size;
    size_t size;
    // void (*free_element)(Element *);
    // void (*copy_element)(Element *, Element *);
    void *elements;
} Array;

Array *array_new(const size_t capacity, const size_t element_size)
{
    Array *a = (Array *)malloc(offsetof(Array, elements));
    a->element_size = element_size;
    a->capacity = 0;
    a->elements = NULL;
    array_increase_capacity(a, capacity);
    a->size = 0;
    return a;
}

void array_free(Array *a)
{
    free(a->elements);
    free(a);
}

size_t array_element_size(Array *const a)
{
    assert(a != NULL);
    return a->element_size;
}

size_t array_size(Array *const a)
{
    assert(a != NULL);
    return a->size;
}

size_t array_capacity(Array *const a)
{
    assert(a != NULL);
    return a->capacity;
}

void array_decrease_capacity(Array *const a, const size_t decrease)
{
    assert(a != NULL);
    if (decrease >= a->capacity)
    {
        free(a->elements);
        a->elements = NULL;
        a->size = 0;
        return;
    }
    a->capacity -= decrease;
    a->elements = (Element *)realloc(a->elements, a->capacity * a->element_size);
    assert(a->elements != NULL);
}

void array_increase_capacity(Array *const a, const size_t increase)
{
    assert(a != NULL);
    a->capacity += increase;
    if (a->elements == NULL)
    {
        a->elements = (Element *)malloc(a->capacity * a->element_size);
        assert(a->elements != NULL);
        return;
    }
    a->elements = (Element *)realloc(a->elements, a->capacity * a->element_size);
    assert(a->elements != NULL);
}

void array_set_capacity(Array *const a, const size_t new_capacity)
{
    if (new_capacity == 0)
    {
        free(a->elements);
        a->elements = NULL;
        a->size = 0;
    }
    else if (a->size > new_capacity)
    {
        array_decrease_capacity(a, a->size - new_capacity);
    }
    else if (a->size < new_capacity)
    {
        array_increase_capacity(a, new_capacity - a->size);
    }
}

void array_push(Array *const a, const Element *const e)
{
    assert(a != NULL);
    if (a->size == a->capacity)
    {
        array_increase_capacity(a, a->capacity);
    }
    memcpy((char *)a->elements + a->size * a->element_size, e, a->element_size);
    a->size++;
}

Element *array_get(Array *const a, const size_t index)
{
    assert(a != NULL);
    assert(index < a->size);
    return (Element *)((char *)a->elements + index * a->element_size);
}

void array_set(Array *const a, const size_t index, const Element *const e)
{
    assert(a != NULL);
    assert(index < a->size);
    memcpy((char *)a->elements + index * a->element_size, e, a->element_size);
}

Element *array_begin(Array *const a)
{
    assert(a != NULL);
    return a->elements;
}

Element *array_end(Array *const a)
{
    assert(a != NULL);
    return (Element *)((char *)a->elements + a->size * a->element_size);
}