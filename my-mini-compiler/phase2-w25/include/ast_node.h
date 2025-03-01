#ifndef AST_NODE_H
#define AST_NODE_H

#include "tokens.h"
#define MAX_AST_CHILDREN 3
#define MAX_ASTNodeType 15

// Basic node types for AST (Abstract Syntax Tree)
typedef enum _ASTNodeType {
    AST_PROGRAM, 
    AST_SCOPE,
    AST_STATEMENTLIST,
    AST_STATEMENT,
    AST_DECLARATION,
    AST_PRINT,
    AST_CODITIONAL,
    AST_WHILE_LOOP,
    AST_REPEATUNTIL_LOOP,
    AST_EXPRESSION,
    AST_FACTORIAL,
    // the nodes below have tokens associated with them.
    AST_OPERATOR, 
    AST_IDENTIFIER,
    AST_INTEGER,
    AST_FLOAT,
    AST_STRING,
    AST_ERROR
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