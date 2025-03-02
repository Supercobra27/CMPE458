#ifndef AST_NODE_H
#define AST_NODE_H

#include "tokens.h"
#define MAX_AST_CHILDREN 3
#define MAX_ASTNodeType 15

// Basic node types for AST (Abstract Syntax Tree)
typedef enum _ASTNodeType {
    // Terminal nodes that have tokens associated with them
    AST_IDENTIFIER,
    AST_TYPE,
    AST_INTEGER,
    AST_FLOAT,
    AST_STRING,
    // Terminal Nodes that are do not have tokens associated with them
    AST_OPERATOR,

    // used by grammar rules to indicate that the ParseToken should be ignored in the AST.
    AST_IGNORE, 
    // used by grammar rules to indicate that the ParseToken children should be passed directly to the parent node.
    AST_FROM_CHILDREN,
    
    // Non-terminal nodes
    AST_PROGRAM, 
    AST_SCOPE,
    AST_STATEMENT_LIST,
    AST_STATEMENT,
    AST_DECLARATION,
    AST_PRINT,
    AST_CODITIONAL,
    AST_WHILE_LOOP,
    AST_REPEAT_UNTIL_LOOP,
    AST_EVALUATE_EXPRESSION,
    AST_EXPRESSION,
    AST_FACTORIAL,
    AST_MUTABLE, // Mutable is needed to indicate that we have to calculate a memory address and store a value there.
    AST_ERROR,
    // used for null-terminated arrays of ASTNodeType
    AST_NULL,
} ASTNodeType;

typedef enum {
    PARSE_ERROR_NONE,
    PARSE_ERROR_UNEXPECTED_TOKEN,
    PARSE_ERROR_MISSING_SEMICOLON,
    PARSE_ERROR_MISSING_IDENTIFIER,
    PARSE_ERROR_MISSING_EQUALS,
    PARSE_ERROR_INVALID_EXPRESSION
} ParseError;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* children[MAX_AST_CHILDREN+1]; // NULL-terminated array of Pointers to child nodes
} ASTNode;

#endif AST_NODE_H