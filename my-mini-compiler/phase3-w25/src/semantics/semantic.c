#include "../../include/semantic.h"
#include <assert.h>

bool isNumeric(ASTNodeType type) {
    return type == AST_INTEGER || type == AST_FLOAT;
}

void ProcessDeclaration(ASTNode *ctx)
{
    printf("Declaration Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    // Check node exists w/ max 2 children
    assert(ctx != NULL);
    assert(ctx->count == 2);

    // Scoping
    char *name = ctx->items[1].token.lexeme;
    char *scope;

    // Check if not already declared within scope. If not add to symbol table.
}

ASTNodeType returnType(ASTNode *ctx) {
    if (ctx->count == 1) return ctx->type;
    ASTNodeType leftType = CHILD_TYPE(ctx, 0);
    ASTNodeType rightType = CHILD_TYPE(ctx, 1);
    assert(leftType == rightType);
    return leftType;
}
// Need to fix multiple operators to check for types

void ProcessOperator(ASTNode *ctx){
    assert(ctx->count == 2); // Binary operator
    ASTNodeType leftType = CHILD_TYPE(ctx, 0);
    ASTNodeType rightType = CHILD_TYPE(ctx, 1);
    if (isNumeric(leftType) && isNumeric(rightType)) assert(leftType == rightType); // ctx->type = returnType(ctx);
    printf("Operator Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    ProcessExpression(&CHILD_ITEM(ctx, 0));
    ProcessExpression(&CHILD_ITEM(ctx, 1));
}

void ProcessUnaryOperator(ASTNode *ctx){
    assert(ctx->count == 1);
    printf("Operator Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    ProcessExpression(&CHILD_ITEM(ctx, 0));
}

void ProcessFunction(ASTNode *ctx){
    printf("Function Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    assert(ctx->count == 1);
    ProcessExpression(ctx->items);

}

// Handles Assignment, Operator, Unary Operator
void ProcessOperation(ASTNode *ctx){
    if (ctx->type == AST_ASSIGN_EQUAL) {
        printf("Assignment Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
        assert(ctx->count == 2);
        assert(CHILD_TYPE(ctx, 0) == AST_IDENTIFIER); // LHS must be an identifier
        assert(CHILD_TYPE(ctx, 1) != AST_ASSIGN_EQUAL); // prevent chained assignment
        ProcessExpression(&CHILD_ITEM(ctx, 1)); // recurse because RHS expression

    } else if (ctx->type >= AST_LOGICAL_OR && ctx->type < AST_BITWISE_NOT) {
        ProcessOperator(ctx);

    } else if (ctx->type >= AST_BITWISE_NOT) {
        ProcessUnaryOperator(ctx);
    }
}

void ProcessExpression(ASTNode *ctx) {
    if (ctx->type == AST_EXPRESSION) ctx = &CHILD_ITEM(ctx, 0);
    switch (ctx->type) {
        case AST_ASSIGN_EQUAL:
        case AST_ADD:
        case AST_SUBTRACT:
        case AST_DIVIDE:
        case AST_MULTIPLY:
        case AST_LOGICAL_OR:
        case AST_LOGICAL_AND:
        case AST_BITWISE_OR:
        case AST_BITWISE_XOR:
        case AST_BITWISE_AND:
        case AST_COMPARE_EQUAL:
        case AST_COMPARE_NOT_EQUAL:
        case AST_COMPARE_LESS_EQUAL:
        case AST_COMPARE_LESS_THAN:
        case AST_COMPARE_GREATER_EQUAL:
        case AST_COMPARE_GREATER_THAN:
        case AST_SHIFT_LEFT:
        case AST_SHIFT_RIGHT:
        case AST_MODULO:
        case AST_BITWISE_NOT:
        case AST_LOGICAL_NOT:
        case AST_NEGATE:
        case AST_FACTORIAL:
            ProcessOperation(ctx);
            break;
        case AST_INTEGER:
        case AST_FLOAT:
        case AST_STRING:
            printf("Literal Analyzing -> %s | %s\n", ASTNodeType_to_string(ctx->type), ctx->token.lexeme);
            break;
        case AST_IDENTIFIER:
            printf("Identifier Analyzing -> %s | %s\n", ASTNodeType_to_string(ctx->type), ctx->token.lexeme);
            break;
        // Scope checking occurs here
        default:
            fprintf(stderr, "Invalid expression node type\n");
    }
}

void ProcessScope(ASTNode *ctx) {
    assert(ctx->type == AST_SCOPE);
    for (size_t child = 0; child < ctx->count; child++) {
        ASTNode *childNode = &ctx->items[child];
        ProcessNode(childNode);
    }
}

void ProcessConditional(ASTNode *ctx) { // If statements
    assert(ctx->type == AST_CODITIONAL);
    assert(ctx->count == 3); // A conditional should have a condition and two scopes

    printf("Conditional Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));

    ASTNodeType condType = CHILD_TYPE(ctx, 0);
    assert(condType == AST_COMPARE_GREATER_THAN || 
           condType == AST_COMPARE_LESS_THAN ||
           condType == AST_COMPARE_EQUAL ||
           condType == AST_COMPARE_NOT_EQUAL ||
           condType == AST_LOGICAL_AND || 
           condType == AST_LOGICAL_OR ||
           condType == AST_LOGICAL_NOT); // Ensure it's a valid comparison (should I add logical?)

    ProcessOperation(&CHILD_ITEM(ctx, 0)); // Process the comparison
    ProcessScope(&CHILD_ITEM(ctx, 1)); // ThenScope
    ProcessScope(&CHILD_ITEM(ctx, 2)); // ElseScope
}

void ProcessLoop(ASTNode *ctx) {
    assert(ctx->type == AST_WHILE_LOOP || ctx->type == AST_REPEAT_UNTIL_LOOP);
    assert(ctx->count == 2);

    printf("Loop Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));

    if (ctx->type == AST_WHILE_LOOP) {
        ProcessExpression(&CHILD_ITEM(ctx, 0));
        ProcessScope(&CHILD_ITEM(ctx, 1));
    }

    if(ctx->type == AST_REPEAT_UNTIL_LOOP) {
        ProcessScope(&CHILD_ITEM(ctx, 0));
        ProcessExpression(&CHILD_ITEM(ctx, 1));
    }
}

// Only export
void ProcessNode(ASTNode *ctx) {
    switch(ctx->type) {
        case AST_SCOPE:
        ProcessScope(ctx);
        break;
        case AST_PROGRAM:
        ProcessProgram(ctx);
        break;
        case AST_CODITIONAL:
        ProcessConditional(ctx);
        break;
        case AST_REPEAT_UNTIL_LOOP:
        case AST_WHILE_LOOP:
        ProcessLoop(ctx);
        break;
        case AST_EXPRESSION:
        ProcessExpression(ctx);
        break;
        case AST_DECLARATION:
        ProcessDeclaration(ctx);
        break;
        case AST_READ:
        case AST_PRINT:
        ProcessFunction(ctx);
        break;
    }
}

void ProcessProgram(ASTNode *head) {
    assert(head->type == AST_PROGRAM);
    printf("Starting Semantic Analysis\n");
    ProcessNode(head->items);
}