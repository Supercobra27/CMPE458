#ifndef AST_NODE_H
#define AST_NODE_H

#include "tokens.h"

// Basic node types for AST (Abstract Syntax Tree)
typedef enum _ASTNodeType {
    // Terminal nodes that have tokens associated with them
    AST_IDENTIFIER,
    AST_INTEGER,
    AST_FLOAT,
    AST_STRING,
    // Terminal Nodes that don't have tokens associated with them. These Nodes will be selected for promotion.
    AST_INT_TYPE,
    AST_FLOAT_TYPE,
    AST_STRING_TYPE,

    // special node types for grammar rules, these will not be present in the AST.
    // used by grammar rules to indicate that the children of this node should not be added to the AST, but the node itself should be. The type of this node must be determined by one of its promoted children. (just take the promotion and ignore the children) (do we want to have a way to specify the node type even if there is no promotion?)
    AST_IGNORE_CHILDREN,
    // used by grammar rules to indicate that the ParseToken and its subtree should be ignored in this particular position in the AST. (node type can still be promoted, if not promoted, then it is ignored entirely, in either case the children are not added to the AST)
    AST_IGNORE,
    // used by grammar rules to indicate that the ParseToken's children should be passed directly to the parent node. Useful when the parsetoken has multiple children and you want them all on the level of the parent node in the AST. Also useful when the children may be empty and in which case nothing is added to the AST.
    AST_FROM_CHILDREN,
    // used when the ParseToken does not have a corresponding ASTNodeType, it's type must be derived from a promoted child, and its children are brought up to the parent as in AST_FROM_CHILDREN. If it's possible to not have promoted children, then this node does not make sense but it will retain the type AST_FROM_PROMOTION (TODO: implement check for this being used incorrectly).
    AST_FROM_PROMOTION,
    
    // Non-terminal nodes
    AST_PROGRAM,
    AST_SCOPE,
    // AST_STATEMENT_LIST, does not exist
    AST_DECLARATION,
    AST_PRINT,
    AST_CODITIONAL,
    AST_WHILE_LOOP,
    AST_REPEAT_UNTIL_LOOP,
    AST_EXPRESSION,

    // actual operation nodes (once promoted)
    AST_ASSIGN_EQUAL,
    AST_LOGICAL_OR,
    AST_LOGICAL_AND,
    AST_BITWISE_OR,
    AST_BITWISE_XOR,
    AST_BITWISE_AND,
    AST_COMPARE_EQUAL,
    AST_COMPARE_NOT_EQUAL,
    AST_COMPARE_LESS_EQUAL,
    AST_COMPARE_LESS_THAN,
    AST_COMPARE_GREATER_EQUAL,
    AST_COMPARE_GREATER_THAN,
    AST_SHIFT_LEFT,
    AST_SHIFT_RIGHT,
    AST_ADD,
    AST_SUBTRACT,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_MODULO,
    AST_BITWISE_NOT,
    AST_LOGICAL_NOT,
    AST_NEGATE,
    AST_FACTORIAL,

    AST_ERROR,
    // used for null-terminated arrays of ASTNodeType
    AST_NULL,
} ASTNodeType;

#define MAX_AST_CHILDREN 3U
#define MAX_ASTNode_WITH_TOKEN AST_STRING
#define ASTNodeType_HAS_TOKEN(type) ((type) <= MAX_ASTNode_WITH_TOKEN)

// These errors should be determined by semantic analysis.
typedef enum {
    AST_ERROR_NONE,
} ASTErrorType;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* children[MAX_AST_CHILDREN+1]; // NULL-terminated array of Pointers to child nodes
} ASTNode;

#endif /* AST_NODE_H */