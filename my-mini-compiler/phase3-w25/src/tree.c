#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../include/tree.h"

#define DA_INITIAL_CAPACITY 256

#define DA_DEFINE(name, type)   \
typedef struct {                \
    type *items;                \
    size_t count;               \
    size_t capacity;            \
} name

// 
/**
 * 
 * @param da The pointer to dynamic array struct to initialize. (must have `items`, `count`, and `capacity` fields).
 */
#define da_init(da) do { (da)->items = NULL; (da)->count = 0; (da)->capacity = 0; } while (0)

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
            if ((da)->capacity == 0) (da)->capacity = DA_INITIAL_CAPACITY;          \
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


// Pop a value from the stack
/**
 * Pop an item from the end of the dynamic array.
 * 
 * @param da The pointer to dynamic array struct to pop from. (must have `items`, `count`, and `capacity` fields).
 */
#define da_pop(da) if ((da)->count > 0) --(da)->count

// Stack structure for booleans
DA_DEFINE(BoolStack, bool);

// Function to print the tree using a stack of booleans for the prefix
void print_tree_rec(print_tree_t *t, BoolStack* stack) {
    const void *const root = t->root;
    if (root == NULL) return;
    // Print the root
    t->print_head(root);
    putc('\n', stdout);
    // Recursively print all children
    const size_t n = t->count(root);
    const void *cp = t->children(root);
    for (size_t i = 0; i < n; ++i) {
        t->root = (char*)cp + i * t->size;
        const bool is_last = (i == n - 1);
        // Print the prefix based on the stack and whether this node is the last child.
        for (size_t i = 0; i < stack->count; ++i)
            printf("%s", stack->items[i] ? "| " : "  ");
        printf("%s", is_last ? "\\-" : "|-");
        // Push the current node's "is_last" status (inverted) onto the stack
        // True means "not last" (pipe), False means "last" (spaces)
        da_push(stack, !is_last);
        print_tree_rec(t, stack);
        da_pop(stack);
    }
    t->root = root; // Reset the root pointer since it was changed in the loop.
}

void print_tree(const print_tree_t *t) {
    if (t == NULL || t->root == NULL || t->children == NULL || t->count == NULL || t->size == 0 || t->print_head == NULL) return;
    // Create a stack and print the tree
    BoolStack stack = {0};
    print_tree_t pt = *t; // Copy the struct to avoid modifying the original (the recursive function does modify the struct, however it reverts those modifications before returning, this is just for safety)
    print_tree_rec(&pt, &stack); // Recursively print the tree
    da_clear(&stack);
}

