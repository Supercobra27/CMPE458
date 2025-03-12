#ifndef SIMPLE_DYNAMIC_ARRAY_H
#define SIMPLE_DYNAMIC_ARRAY_H

#define DA_INITIAL_CAPACITY 256

#define DA_DEFINE(name, type)   \
typedef struct {                \
    type *items;                \
    size_t capacity;            \
    size_t count;               \
} name

// 
/**
 * 
 * @param da The pointer to dynamic array struct to initialize. (must have `items`, `count`, and `capacity` fields).
 */
#define da_init(da) do { (da)->items = NULL; (da)->capacity = 0; (da)->count = 0; } while (0)

// 
/**
 * Push (append) an item to the end of the dynamic array.
 * 
 * @param da The pointer to dynamic array struct to push to. (must have `items`, `count`, and `capacity` fields).
 * @param item The item to push.
 */
#define da_push(da, item)                                                           \
    do                                                                              \
    {                                                                               \
        if ((da)->count >= (da)->capacity) {                                        \
            if ((da)->capacity == 0) (da)->capacity = 1;                            \
            else (da)->capacity *= 2;                                               \
            (da)->capacity *= 2;                                                    \
            void *ptr = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));\
            if (ptr == NULL) {                                                      \
                perror("realloc");                                                  \
                free((da)->items);                                                  \
                exit(EXIT_FAILURE);                                                 \
            }                                                                       \
            (da)->items = ptr;                                                      \
        }                                                                           \
        (da)->items[(da)->count++] = (item);                                        \
    } while (0)

// 
/**
 * Clear the dynamic array, freeing the items and setting the count and capacity to 0.
 * 
 * @param da The pointer to dynamic array struct to clear. (must have `items`, `count`, and `capacity` fields).
 */
#define da_clear(da)                            \
    do {                                        \
        free((da)->items);                      \
        (da)->items = NULL;                     \
        (da)->count = 0;                        \
        (da)->capacity = 0;                     \
    } while (0)

#endif /* SIMPLE_DYNAMIC_ARRAY_H */