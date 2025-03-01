/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

#define MAX_AST_CHILDREN 3

// Basic node types for AST
typedef enum _ASTNodeType {
    AST_PROGRAM, // 1 child (statement_list)
    AST_STATEMENT_LIST, // 2/0 children (statement, statement_list | empty)
    AST_STATEMENT, // 1 child (statement)
    AST_EMPTY_STATEMENT, // 0 children (empty)
    AST_DECLARATION, // 2 children (type, identifier)
    AST_EXPRESSION_STATEMENT, // 1 child (expression)
    AST_PRINT_STATEMENT, // 1 child (expression)
    AST_BLOCK, // 1 child (statement_list)
    AST_CONDITIONAL_STATEMENT, // 1 child (IF_then_else | if_then)
    AST_IF_THEN_ELSE, // 3 children (expression, block, block)
    AST_IF_THEN, // 2 children (expression, block)
    AST_WHILE_LOOP, // 2 children (expression, block)
    AST_REPEAT_UNTIL_LOOP, // 2 children (block, expression)
    // Expression-level nodes
    AST_EXPRESSION, // 1 child
    AST_ASSIGNMENT_EXPR_r12, // 2 children (identifier, expression)
    AST_OR_EXPR_l11, // 2 children (expression, expression)
    AST_AND_EXPR_l10, // 2 children (expression, expression)
    AST_BIT_OR_EXPR_l9, // 2 children (expression, expression)
    AST_BIT_XOR_EXPR_l8, // 2 children (expression, expression)
    AST_BIT_AND_EXPR_l7, // 2 children (expression, expression)
    AST_RELATION_EXPR_l6, // 2 children (expression, expression)
    AST_SHIFT_EXPR_l5, // 2 children (expression, expression)
    AST_SUM_EXPR_l4, // 2 children (expression, expression)
    AST_PROD_EXPR_l3, // 2 children (expression, expression)
    AST_UNARY_PREFIX_EXPR_l2, // 1 child (expression)
    AST_FACTOR_EXPR_l0, // 1 child (expression)
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

// Parser functions
void parser_init(const char* input);
ASTNode* parse(void);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);

#endif /* PARSER_H */