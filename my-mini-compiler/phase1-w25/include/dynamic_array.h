#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

typedef struct Array_t Array;
typedef void *Element;

// Array *array_new(size_t capacity, size_t element_size, void (*free_element)(Element *), void (*copy_element)(Element *, Element *));

/**
 * Allocates a new array with the given capacity and element size.
 * @param capacity The initial capacity of the array.
 * @param element_size The size of the elements in the array.
 * @return A pointer to the newly allocated array.
 */
Array *array_new(size_t capacity, size_t element_size);

/**
 * Frees the memory allocated for the array.
 * @param a The array to free.
 */
void array_free(Array *a);

/**
 * Returns the size of the elements in the array.
 * @param a The array to get the element size of. If `a` is NULL, Undefined behavior.
 */
size_t array_element_size(Array *const a);

/**
 * Returns the number of elements in the array.
 * @param a The array to get the size of. If `a` is NULL, Undefined behavior.
 */
size_t array_size(Array *const a);

/**
 * Returns the capacity of the array.a
 * @param a The array to get the capacity of. If `a` is NULL, Undefined behavior.
 */
size_t array_capacity(Array *const a);

/**
 * Increases the capacity of the array by the given amount.
 * @param a The array to increase the capacity of. If `a` is NULL, Undefined behavior.
 */
void array_increase_capacity(Array *const a, const size_t increase);

/**
 * Decreases the capacity of the array by the given amount.
 * @param a The array to decrease the capacity of. If `a` is NULL, Undefined behavior.
 */
void array_decrease_capacity(Array *const a, const size_t decrease);

/**
 * Resizes the array to the new capacity.
 * If `new_capacity` is less than `a->size`, the array is truncated and the truncated elements are freed.
 * @param a The array to resize.
 * @param new_capacity The new capacity of the array. If new_capacity == 0, the array elements are all freed.
 */
void array_set_capacity(Array *const a, const size_t new_capacity);

/**
 * Inserts an element at the end of the array by memcpy, resizing if necessary.
 * @param a The array.
 * @param e The element to push.
 */
void array_push(Array *const a, const Element *const e);

/**
 * Accesses the element at the given index.
 * @param a The array to access.
 * @param index The index of the element to access. (0 <= index < array_size(a))
 * @return The element at the given index. Undefined behavior if the index is out of bounds.
 */
Element array_get(Array *const a, const size_t index);

/**
 * Sets the element at the given index by using memcpy.
 * @param a The array to set.
 * @param index The index of the element to set.
 * @param e The element to be copied into the array.
 */
void array_set(Array *const a, const size_t index, const Element *const e);

/**
 * Returns a pointer to the first element in the array.
 * @param a The array to get the first element of.
 * @return A pointer to the first element in the array.
 */
Element *array_begin(Array *const a);

/**
 * Returns a pointer to the element after the last element in the array.
 * @param a The array to get the end of.
 * @return A pointer to the element after the last element in the array. (`array_end(a) + array_size(a)` is equivalent to what is returned)
 */
Element *array_end(Array *const a);

// Could add ability to create elements in place by passing a function pointer if needed.

#endif