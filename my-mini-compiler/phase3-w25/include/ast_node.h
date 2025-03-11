#ifndef AST_NODE_H
#define AST_NODE_H

#include "tokens.h"
#include <stddef.h> // size_t

// Basic node types for AST (Abstract Syntax Tree)
typedef enum _ASTNodeType {
    // used for null-terminated arrays of ASTNodeType
    AST_NULL,
    // Terminal nodes that have tokens associated with them
    AST_IDENTIFIER,
    AST_INTEGER,
    AST_FLOAT,
    AST_STRING,
    // Terminal Nodes that don't have tokens associated with them.
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
    AST_SCOPE, // StatementList@ (variable number of children)
    // AST_STATEMENT_LIST, does not exist
    AST_DECLARATION, // TYPE IDENTIFIER
    AST_PRINT, // Operation
    AST_READ,  // Operation
    AST_CODITIONAL, // Operation SCOPE SCOPE
    AST_WHILE_LOOP, // Operation SCOPE
    AST_REPEAT_UNTIL_LOOP, // Operation SCOPE
    AST_EXPRESSION, // Operation
    // Binary Operations
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
    // Unary Operations
    AST_BITWISE_NOT,
    AST_LOGICAL_NOT,
    AST_NEGATE,
    AST_FACTORIAL,
} ASTNodeType;

#define ASTNodeType_HAS_TOKEN(type) (AST_NULL < (type) && (type) <= AST_STRING)
#define ASTNodeType_IS_TERMINAL(type) (AST_NULL < (type) && (type) <= AST_STRING_TYPE)
#define ASTNodeType_IS_NONTERMINAL(type) (AST_PROGRAM <= (type) && (type) <= AST_FACTORIAL)
#define ASTNodeType_IS_BINARY(type) (AST_ASSIGN_EQUAL <= (type) && (type) <= AST_MODULO)
#define ASTNodeType_IS_UNARY(type) (AST_BITWISE_NOT <= (type) && (type) <= AST_NEGATE)

static const char *ASTNodeType_to_string(const ASTNodeType t)
{
    switch(t)
    {
        case AST_NULL:
            return "AST_NULL";
        case AST_IDENTIFIER:
        // Terminal Nodes
            return "AST_IDENTIFIER";
        case AST_INTEGER:
            return "AST_INTEGER";
        case AST_FLOAT:
            return "AST_FLOAT";
        case AST_STRING:
            return "AST_STRING";
        case AST_INT_TYPE:
            return "AST_INT_TYPE";
        case AST_FLOAT_TYPE:    
            return "AST_FLOAT_TYPE";
        case AST_STRING_TYPE:
            return "AST_STRING_TYPE";
        // Special Node Types
        case AST_IGNORE_CHILDREN:
            return "AST_IGNORE_CHILDREN";
        case AST_IGNORE:
            return "AST_IGNORE";
        case AST_FROM_CHILDREN:
            return "AST_FROM_CHILDREN";
        case AST_FROM_PROMOTION:
            return "AST_FROM_PROMOTION";
        // Non-terminal Nodes
        case AST_PROGRAM:
            return "AST_PROGRAM";
        case AST_SCOPE:
            return "AST_SCOPE";
        case AST_DECLARATION:
            return "AST_DECLARATION";
        case AST_PRINT:
            return "AST_PRINT";
        case AST_READ:
            return "AST_READ";
        case AST_CODITIONAL:
            return "AST_CODITIONAL";
        case AST_WHILE_LOOP:
            return "AST_WHILE_LOOP";
        case AST_REPEAT_UNTIL_LOOP:
            return "AST_REPEAT_UNTIL_LOOP";
        case AST_EXPRESSION:
            return "AST_EXPRESSION";
        case AST_ASSIGN_EQUAL:
            return "AST_ASSIGN_EQUAL";
        case AST_LOGICAL_OR:
            return "AST_LOGICAL_OR";
        case AST_LOGICAL_AND:
            return "AST_LOGICAL_AND";
        case AST_BITWISE_OR:
            return "AST_BITWISE_OR";
        case AST_BITWISE_XOR:
            return "AST_BITWISE_XOR";
        case AST_BITWISE_AND:
            return "AST_BITWISE_AND";
        case AST_COMPARE_EQUAL:
            return "AST_COMPARE_EQUAL";
        case AST_COMPARE_NOT_EQUAL:
            return "AST_COMPARE_NOT_EQUAL";
        case AST_COMPARE_LESS_EQUAL:
            return "AST_COMPARE_LESS_EQUAL";
        case AST_COMPARE_LESS_THAN:
            return "AST_COMPARE_LESS_THAN";
        case AST_COMPARE_GREATER_EQUAL:
            return "AST_COMPARE_GREATER_EQUAL";
        case AST_COMPARE_GREATER_THAN:
            return "AST_COMPARE_GREATER_THAN";
        case AST_SHIFT_LEFT:
            return "AST_SHIFT_LEFT";
        case AST_SHIFT_RIGHT:
            return "AST_SHIFT_RIGHT";
        case AST_ADD:
            return "AST_ADD";
        case AST_SUBTRACT:
            return "AST_SUBTRACT";
        case AST_MULTIPLY:
            return "AST_MULTIPLY";
        case AST_DIVIDE:
            return "AST_DIVIDE";
        case AST_MODULO:
            return "AST_MODULO";
        case AST_BITWISE_NOT:
            return "AST_BITWISE_NOT";
        case AST_LOGICAL_NOT:
            return "AST_LOGICAL_NOT";
        case AST_NEGATE:
            return "AST_NEGATE";
        case AST_FACTORIAL:
            return "AST_FACTORIAL";
    }
    return "UNKNOWN";
}

// These errors should be determined by semantic analysis.
typedef enum {
    AST_ERROR_NONE,
} ASTErrorType;

static const char *ASTErrorType_to_string(const ASTErrorType t)
{
    switch(t)
    {
        case AST_ERROR_NONE:
            return "AST_ERROR_NONE";
    }
    return "UNKNOWN";
}

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    ASTErrorType error;         // Error type
    Token token;               // Token associated with this node, TOKEN_NULL if none.
    size_t count;
    size_t capacity;
    struct ASTNode *items; // Array of child nodes
} ASTNode;

#endif /* AST_NODE_H */