#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../include/tree.h"

#include "../include/simple_dynamic_array.h"

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
    if (root == NULL) 
        return;
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

