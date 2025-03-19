#include "semantic.h"
#include <assert.h>

void ProcessOperator(ASTNode *ctx){
    assert(ctx->count == 2); // Binary operator
    ASTNodeType leftType = CHILD_TYPE(0);
    ASTNodeType rightType = CHILD_TYPE(1);

    assert(leftType == rightType); // Ensure type consistency
    assert(leftType == AST_INTEGER || leftType == AST_FLOAT); // Only numerical types

    ProcessExpression(&CHILD_ITEM(0));
    ProcessExpression(&CHILD_ITEM(1));
}

void ProcessExpression(ASTNode *ctx) {
    switch (ctx->type) {
        case AST_ASSIGN_EQUAL:
            assert(ctx->count == 2);
            assert(CHILD_TYPE(0) == AST_IDENTIFIER); // LHS must be an identifier
            assert(CHILD_TYPE(1) != AST_ASSIGN_EQUAL); // prevent chained assignment
            ProcessExpression(&CHILD_ITEM(1)); //recurse because RHS expression
            break;
        case AST_ADD:
            ProcessOperator(ctx);
            break;
        case AST_SUBTRACT:
            ProcessOperator(ctx);
            break;
        case AST_MULTIPLY:
            ProcessOperator(ctx);
            break;
        case AST_DIVIDE:
            ProcessOperator(ctx);
            break;
        case AST_IDENTIFIER:
        case AST_INTEGER:
        case AST_FLOAT:
        case AST_STRING:
            break;
        default:
            fprintf(stderr, "Invalid expression node type\n");
            exit(1);
    }
}

void ProcessScope(ASTNode *ctx) {
    assert(ctx->type == AST_SCOPE);
    for (int child = 0; child < ctx->count; child++) {
        ASTNode *childNode = &ctx->items[child];
        switch (childNode->type) {
            // Insert Functions here
        }
    }
}

void ProcessConditional(ASTNode *ctx) { // If statements
    assert(ctx->type == AST_CODITIONAL);
    assert(ctx->count == 3); // A conditional should have a condition and two scopes

    ASTNodeType condType = CHILD_TYPE(0);
    assert(condType == AST_COMPARE_GREATER_THAN || 
           condType == AST_COMPARE_LESS_THAN ||
           condType == AST_COMPARE_EQUAL ||
           condType == AST_COMPARE_NOT_EQUAL ||
           condType == AST_LOGICAL_AND || 
           condType == AST_LOGICAL_OR ||
           condType == AST_LOGICAL_NOT); // Ensure it's a valid comparison (should I add logical?)

    ProcessOperator(&CHILD_ITEM(0)); // Process the comparison
    ProcessScope(&CHILD_ITEM(1)); // ThenScope
    ProcessScope(&CHILD_ITEM(2)); // ElseScope
}


void ProcessAssignment(ASTNode *ctx); // Ryan
void ProcessDeclaration(ASTNode *ctx); // Simon
void ProcessExpression(ASTNode *ctx); // Ryan
void ProcessConditional(ASTNode *ctx); // Ryan operator scope scope
void ProcessFunction(ASTNode *ctx); // Simon
void ProcessScope(ASTNode *ctx); // Simon
void ProcessString(ASTNode *ctx); // Simon