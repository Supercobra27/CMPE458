#ifndef TREE_H
#define TREE_H

typedef const void *(const_voidp_to_const_voidp)(const void *);
typedef size_t (const_voidp_to_size_t)(const void *);
typedef void (const_voidp_to_void)(const void *);

/**
 * Function pointers for printing a tree.
 * 
 * @param root The root of the tree to print.
 * @param children_begin A function that returns the beginning of the children of a node.
 * @param num_children A function that returns the number of children of a node.
 * @param size The size of the node in bytes. 
 * @param print_head A function that prints the head of a node
 */
typedef struct _print_tree_t
{
    const void *root;
    const_voidp_to_const_voidp *children_begin;
    const_voidp_to_size_t *num_children;
    const size_t size;
    const_voidp_to_void *print_head;
} print_tree_t;

/**
 * Print a tree starting from the root node to stdout.
 * 
 * Example output:
 * ```
 * root
 * |-child1
 * | |-child1.1
 * | \-child1.2
 * \-child2
 *   \-child2.1
 * ```
 * 
 * @param tf parameters for printing the tree.
 */
void print_tree(const print_tree_t *t);

#endif /* TREE_H */