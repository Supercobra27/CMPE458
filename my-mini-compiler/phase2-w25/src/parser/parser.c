/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/parse_tokens.h"
#include "../../include/grammar.h"
#include "../../include/parser.h"

/*
TODO: Delete everything below, and implement just one function to parse according to a grammar rule.abort

We have to define the grammar rules for parsing in a data structure. Grammar is an array(indexed by ParseToken enum) of GrammarRule, GrammarRule is an array of ProductionRule. ProductionRule is an array of ParseToken. 

parsing will be recursive descent parsing. we need two functions to accomplish this:
- right-recursive parsing function
- left-recursive parsing function (only for operator grammar that does not have indirect left recursion https://www.geeksforgeeks.org/removing-direct-and-indirect-left-recursion-in-a-grammar/)

Errors will all be of the form:
- at the token level: just say missing/wrong token (e.g. trying to parse a PT_THEN_KEYWORD and the token is TOKEN_ELSE, then say "expected THEN, got ELSE") Also, do we consume this token, or leave it for the rest of the parser?
- at the grammar level: say what is missing/unable to parse (e.g. trying to parse a PT_IF_THEN_ELSE and the first token is PT_IF_KEYWORD, but the next token is not a PT_EXPRESSION, then say "expected expression after IF, got something else").

*/

// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;

static void parse_error(ParseError error, Token token)
{
    // TODO 2: Add more error types for:
    // - Missing parentheses
    // - Missing condition
    // - Missing block braces
    // - Invalid operator
    // - Function call errors

    printf("Parse Error at line %d: ", token.line);
    switch (error)
    {
    case PARSE_ERROR_UNEXPECTED_TOKEN:
        printf("Unexpected token '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_SEMICOLON:
        printf("Missing semicolon after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_IDENTIFIER:
        printf("Expected identifier after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_EQUALS:
        printf("Expected '=' after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_INVALID_EXPRESSION:
        printf("Invalid expression after '%s'\n", token.lexeme);
        break;
    default:
        printf("Unknown error\n");
    }
}

// Get next token
static void advance(void)
{
    current_token = get_next_token(source, &position);
}

// Create a new AST node
static ASTNode *create_node(ASTNodeType type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node)
    {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

// Match current token with expected type
static int match(TokenType type)
{
    return current_token.type == type;
}

// Expect a token type or error
static void expect(TokenType type)
{
    if (match(type))
    {
        advance();
    }
    else
    {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1); // Or implement error recovery
    }
}

// Forward declarations
static ASTNode *parse_statement(void);

// TODO 3: Add parsing functions for each new statement type
// static ASTNode* parse_if_statement(void) { ... }
// static ASTNode* parse_while_statement(void) { ... }
// static ASTNode* parse_repeat_statement(void) { ... }
// static ASTNode* parse_print_statement(void) { ... }
// static ASTNode* parse_block(void) { ... }
// static ASTNode* parse_factorial(void) { ... }

static ASTNode *parse_expression(void);

// Parse variable declaration: int x;
// The token stored in this ASTNode is the identifier.
static ASTNode *parse_declaration(void)
{
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume 'int'

    if (!match(TOKEN_IDENTIFIER))
    {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        exit(1);
    }

    node->token = current_token;
    advance();

    if (!match(TOKEN_SEMICOLON))
    {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse assignment: x = 5;
// The left child is the identifier, the right child is the expression.
// Nothing is stored for the token of this ASTNode (should be the equals operator).
static ASTNode *parse_assignment(void)
{
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    if (!match(TOKEN_EQUALS))
    {
        parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
        exit(1);
    }
    advance();

    node->right = parse_expression();

    if (!match(TOKEN_SEMICOLON))
    {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse statement
// Currently only handles declarations and assignments
static ASTNode *parse_statement(void)
{
    if (match(TOKEN_INT))
    {
        return parse_declaration();
    }
    else if (match(TOKEN_IDENTIFIER))
    {
        return parse_assignment();
    }

    // TODO 4: Add cases for new statement types
    // else if (match(TOKEN_IF)) return parse_if_statement();
    // else if (match(TOKEN_WHILE)) return parse_while_statement();
    // else if (match(TOKEN_REPEAT)) return parse_repeat_statement();
    // else if (match(TOKEN_PRINT)) return parse_print_statement();
    // ...

    printf("Syntax Error: Unexpected token\n");
    exit(1);
}

// Parse expression (currently only handles numbers and identifiers)
// No token is stored in the ASTNode. The token should be stored somehow in the ASTNode.

// TODO 5: Implement expression parsing
// Current expression parsing is basic. Need to implement:
// - Binary operations (+-*/)
// - Comparison operators (<, >, ==, etc.)
// - Operator precedence
// - Parentheses grouping
// - Function calls

static ASTNode *parse_expression(void)
{
    ASTNode *node;

    if (match(TOKEN_NUMBER))
    {
        node = create_node(AST_NUMBER);
        advance();
    }
    else if (match(TOKEN_IDENTIFIER))
    {
        node = create_node(AST_IDENTIFIER);
        advance();
    }
    else
    {
        printf("Syntax Error: Expected expression\n");
        exit(1);
    }

    return node;
}

// Parse program (multiple statements)
// This parses the statements as a linked list, the left child is the current statement, the right chid points to the next statement.
// There is no parent node for the program, the root node is the first statement.
// Statement Nodes don't have a token associated with them.
static ASTNode *parse_program(void)
{
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF))
    {
        current->left = parse_statement();
        if (!match(TOKEN_EOF))
        {
            current->right = create_node(AST_PROGRAM);
            current = current->right;
        }
    }

    return program;
}

// Initialize parser
void parser_init(const char *input)
{
    source = input;
    position = 0;
    advance(); // Get first token
}

// Main parse function
ASTNode *parse(void)
{
    return parse_program();
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int level)
{
    if (!node)
        return;

    // Indent based on level
    for (int i = 0; i < level; i++)
        printf("  ");

    // Print node info
    switch (node->type)
    {
    case AST_PROGRAM:
        printf("Program\n");
        break;
    case AST_VARDECL:
        printf("VarDecl: %s\n", node->token.lexeme);
        break;
    case AST_ASSIGN:
        printf("Assign\n");
        break;
    case AST_NUMBER:
        printf("Number: %s\n", node->token.lexeme);
        break;
    case AST_IDENTIFIER:
        printf("Identifier: %s\n", node->token.lexeme);
        break;

    // TODO 6: Add cases for new node types
    // case AST_IF: printf("If\n"); break;
    // case AST_WHILE: printf("While\n"); break;
    // case AST_REPEAT: printf("Repeat-Until\n"); break;
    // case AST_BLOCK: printf("Block\n"); break;
    // case AST_BINOP: printf("BinaryOp: %s\n", node->token.lexeme); break;
    default:
        printf("Unknown node type\n");
    }

    // Print children
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

// Free AST memory
void free_ast(ASTNode *node)
{
    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}


// Main function for testing
int main()
{
    GrammarRule grammar[COUNT_NONTERMINAL_ParseToken] = {
        (GrammarRule){
            .lhs = PT_PROGRAM,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT_LIST, PT_EOF, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_STATEMENT_LIST, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_STATEMENT_LIST,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT, PT_STATEMENT_LIST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_STATEMENT, AST_STATEMENT_LIST, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EMPTY_STATEMENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DECLARATION, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_DECLARATION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EXPRESSION_STATEMENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRINT_STATEMENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_PRINT, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_SCOPE, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_CONDITIONAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_CODITIONAL, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_WHILE_LOOP, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_WHILE_LOOP, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_REPEAT_UNTIL_LOOP, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_REPEAT_UNTIL_LOOP, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_EMPTY_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_DECLARATION,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_TYPE_KEYWORD, PT_IDENTIFIER, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_TYPE, AST_IDENTIFIER, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_EXPRESSION_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EXPRESSION, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_PRINT_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRINT_KEYWORD, PT_EXPRESSION, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BLOCK,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BLOCK_BEGIN, PT_STATEMENT_LIST, PT_BLOCK_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_STATEMENT_LIST, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_CONDITIONAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_IF_KEYWORD, PT_EXPRESSION, PT_THEN, PT_BLOCK, PT_OPTIONAL_ELSE_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTED_CHILD, AST_IGNORE, AST_SCOPE, AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_WHILE_LOOP,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_WHILE_KEYWORD, PT_EXPRESSION, PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_SCOPE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_REPEAT_UNTIL_LOOP,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_REPEAT_KEYWORD, PT_BLOCK, PT_UNTIL_KEYWORD, PT_EXPRESSION, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_SCOPE, AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_OPTIONAL_ELSE_BLOCK,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ELSE_KEYWORD, PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_SCOPE, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NULL}},
                NULL},
        },

        (GrammarRule){
            .lhs = PT_STATEMENT_END,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SEMICOLON, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_TYPE_KEYWORD,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_INT_KEYWORD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_TYPE, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FLOAT_KEYWORD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_TYPE, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STRING_KEYWORD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FLOAT, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_EXPRESSION,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ASSIGNMENTEX_R12, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BLOCK_BEGIN,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LEFT_BRACE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BLOCK_END,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_RIGHT_BRACE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        // TODO: fix these rules
        (GrammarRule){
            .lhs = PT_ASSIGNMENTEX_R12,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MUTABLE, PT_ASSIGNMENT_OPERATOR, PT_OREX_L11, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_PROMOTE, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_OREX_L11,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_OR_OPERATOR, PT_ANDEX_L10, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ANDEX_L10, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_ANDEX_L10,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ANDEX_L10, PT_AND_OPERATOR, PT_BITOREX_L9, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITOREX_L9, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITOREX_L9,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITOREX_L9, PT_BITOR_OPERATOR, PT_BITXOREX_L8, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITXOREX_L8, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITXOREX_L8,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITXOREX_L8, PT_BITXOR_OPERATOR, PT_BITANDEX_L7, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITANDEX_L7, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITANDEX_L7,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITANDEX_L7, PT_BITAND_OPERATOR, PT_RELATIONEX_L6, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_RELATIONEX_L6, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_RELATIONEX_L6,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_RELATIONEX_L6, PT_RELATIONAL_OPERATOR, PT_SHIFTEX_L5, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFTEX_L5, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SHIFTEX_L5,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFTEX_L5, PT_SHIFT_OPERATOR, PT_SUMEX_L4, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SUMEX_L4, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SUMEX_L4,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SUMEX_L4, PT_SUM_OPERATOR, PT_PRODUCTEX_L3, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRODUCTEX_L3, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_PRODUCTEX_L3,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRODUCTEX_L3, PT_PRODUCT_OPERATOR, PT_UNARYPREFIXEX_R2, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_UNARYPREFIXEX_R2, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_UNARYPREFIXEX_R2,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_UNARY_PREFIX_OPERATOR, PT_UNARYPREFIXEX_R2, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_EXPRESSION, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FACTOR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_FACTOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_IMMUTABLE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MUTABLE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_MUTABLE, AST_NULL}},
                NULL},
        },

        (GrammarRule){
            .lhs = PT_ASSIGNMENT_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ASSIGN_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_ASSIGN_EQUAL, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_OR_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LOGICAL_OR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_LOGICAL_OR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_AND_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LOGICAL_AND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_LOGICAL_AND, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITOR_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_OR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_OR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITXOR_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_XOR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_XOR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITAND_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_AND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_AND, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_RELATIONAL_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_LESS_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_LESS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_GREATER_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_GREATER, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_NOT_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SHIFT_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFT_LEFT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFT_RIGHT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SUM_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ADD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SUBTRACT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_PRODUCT_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MULTIPLY, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DIVIDE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MODULO, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_UNARY_PREFIX_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_NOT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LOGICAL_NOT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NEGATE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_OPERATOR, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_ASSIGN_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SINGLE_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_LOGICAL_OR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_PIPE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_LOGICAL_AND,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_AMPERSAND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITWISE_OR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SINGLE_PIPE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITWISE_XOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_CARET, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITWISE_AND,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SINGLE_AMPERSAND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_COMPARE_LESS_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LESS_THAN_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_COMPARE_LESS,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LESS_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_COMPARE_GREATER_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_GREATER_THAN_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_COMPARE_GREATER,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_GREATER_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_COMPARE_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_COMPARE_NOT_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BANG_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SHIFT_LEFT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_LESS_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SHIFT_RIGHT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_GREATER_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_ADD,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PLUS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_SUBTRACT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MINUS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_MULTIPLY,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STAR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_DIVIDE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FORWARD_SLASH, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_MODULO,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PERCENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_BITWISE_NOT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_TILDE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_LOGICAL_NOT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BANG, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_NEGATE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MINUS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL}},
                NULL},
        },

        (GrammarRule){
            .lhs = PT_IMMUTABLE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LEFT_PAREN, PT_EXPRESSION, PT_RIGHT_PAREN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_CONSTANT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FACTORIAL_CALL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FACTORIAL, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_MUTABLE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_IDENTIFIER, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IDENTIFIER, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_FACTORIAL_CALL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FACTORIAL_KEYWORD, PT_LEFT_PAREN, PT_EXPRESSION, PT_RIGHT_PAREN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL}},
                NULL},
        },
        (GrammarRule){
            .lhs = PT_CONSTANT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_INTEGER_CONST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_INTEGER, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FLOAT_CONST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FLOAT, AST_NULL}},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STRING_CONST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_STRING, AST_NULL}},
                NULL},
        },
    };

    // Validate Grammar
    for (ParseToken i = FIRST_NONTERMINAL_ParseToken; i < FIRST_NONTERMINAL_ParseToken + COUNT_NONTERMINAL_ParseToken; ++i)
    {
        // ensure the grammar has a rule for each non-terminal in the correct index
        if (grammar[i-FIRST_NONTERMINAL_ParseToken].lhs != i)
        {
            printf("Grammar rule for %d is missing\n", i);
            exit(1);
        }
    }



    // Test with both valid and invalid inputs
    const char *input = "int x;\n"   // Valid declaration
                        "x = 42;\n"; // Valid assignment;
    // TODO 8: Add more test cases and read from a file:
    const char *invalid_input = "int x;\n"
                                "x = 42;\n"
                                "int ;";

    printf("Parsing input:\n%s\n", invalid_input);
    parser_init(invalid_input);
    ASTNode *ast = parse();

    printf("\nAbstract Syntax Tree:\n");
    print_ast(ast, 0);

    free_ast(ast);
    return 0;
}
