#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/tree.h"

// Stack structure for booleans
typedef struct BoolStack {
    bool* data;                     // Dynamic array of booleans
    size_t size;                       // Current number of elements
    size_t capacity;                   // Total allocated capacity
} BoolStack;

// Initialize the stack
BoolStack* create_stack(size_t initial_capacity) {
    BoolStack* stack = (BoolStack*)malloc(sizeof(BoolStack));
    stack->data = (bool*)malloc(initial_capacity * sizeof(bool));
    stack->size = 0;
    stack->capacity = initial_capacity;
    return stack;
}

// Push a value onto the stack
void push(BoolStack* stack, bool value) {
    if (stack->size >= stack->capacity) {
        stack->capacity *= 2;
        stack->data = (bool*)realloc(stack->data, stack->capacity * sizeof(bool));
        if (!stack->data) {
            fprintf(stderr, "Stack realloc failed\n");
            exit(1);
        }
    }
    stack->data[stack->size++] = value;
}

// Pop a value from the stack
void pop(BoolStack* stack) {
    if (stack->size > 0) {
        stack->size--;
    }
}

// Free the stack
void free_stack(BoolStack* stack) {
    free(stack->data);
    free(stack);
}

// Function to print the tree using a stack of booleans for the prefix
void print_tree_rec(print_tree_t *t, BoolStack* stack, bool is_last) {
    const void *const root = t->root;
    if (root == NULL) return;
    
    // Print the current node
    t->print_head(root);

    // Only process children if there are any
    size_t n = t->num_children(root);
    if (n == 0) return;

    // Push the current node's "is_last" status (inverted) onto the stack
    // True means "not last" (pipe), False means "last" (spaces)
    push(stack, !is_last);

    // Recursively print all children
    const void *cp = t->children_begin(root);
    for (size_t i = 0; i < n; i++) {
        t->root = (char*)cp + i * t->size;
        // Print the prefix based on the stack and whether this node is the last child.
        for (size_t i = 1; i < stack->size; ++i)
            printf("%s", stack->data[i] ? "| " : "  ");
        printf("%s", is_last ? "\\-" : "|-");
        print_tree_rec(t, stack, (i == n - 1));
    }
    t->root = root; // Reset the root pointer since it was changed in the loop.

    // Pop the current level off the stack
    pop(stack);
}

void print_tree(const print_tree_t *t)
{
    if (t == NULL || t->root == NULL || t->children_begin == NULL || t->num_children == NULL || t->size == 0 || t->print_head == NULL) return;
    // Create a stack and print the tree
    BoolStack* stack = create_stack(16);
    print_tree_t pt = *t; // Copy the struct to avoid modifying the original (the recursive function does modify the struct, however it reverts those modifications before returning, this is just for safety)
    print_tree_rec(&pt, stack, true); // Root is treated as "last" for simplicity
    free_stack(stack);
}

