#include "../../include/semantic.h"
#include "../../include/dynamic_array.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


void ProcessExpression(ASTNode *ctx, Array *symbol_table);
void ProcessDeclaration(ASTNode *ctx, Array *symbol_table);
void ProcessOperation(ASTNode *ctx, Array *symbol_table);
void ProcessProgram(ASTNode *head, Array *symbol_table);
void ProcessNode(ASTNode *ctx, Array *symbol_table);

/**
 * Implement a stack where everytime you enter a new scope, put a 0 on the stack, the size of the stack is the current scope to be stored
 * remove one upon leaving store in symbol table. Treat it like coordinates, i.e. start at just 0, enter another scope becomes 0.0
 * To check, since they are prefix free, we can just match.
*/

// Global scope stack
static Array* scopeStack = NULL;
static int* scopeCounters = NULL;  // Array of levels to keep track of highest scope at each level
static int maxStackSize = 50;      // Maximum nesting level

// Initialize the scope tracking system
void InitializeScopeStack() {

    scopeStack = array_new(10, sizeof(int*));  // Initial capacity of 10
    
    scopeCounters = (int*)malloc(maxStackSize * sizeof(int));
    if (!scopeCounters) {
        fprintf(stderr, "Failed to allocate memory for scope counters\n");
        exit(1);
    }
    
    // Initialize all counters to 0
    for (int i = 0; i < maxStackSize; i++) {
        scopeCounters[i] = 0;
    }
    
    // Start with the root scope "0"
    int* initialScope = (int*)malloc(sizeof(int));
    *initialScope = 0;
    array_push(scopeStack, (Element*)initialScope);
}

void EnterScope() {
    int stackSize = array_size(scopeStack);
    if (stackSize >= maxStackSize) {
        fprintf(stderr, "Maximum scope nesting level exceeded\n");
        return;
    }
    
    // Get next counter value for this level
    int* newScope = (int*)malloc(sizeof(int));
    *newScope = scopeCounters[stackSize]++;
    array_push(scopeStack, (Element*)newScope);
}

void ExitScope() {
    if (array_size(scopeStack) <= 1) {
        fprintf(stderr, "Cannot exit from root scope\n");
        return;
    }
    
    array_pop(scopeStack);
    // TODO: use array_decrease_capacity to shrink the stack if necessary
}

// Get the current scope as a string
char* GetCurrentScope() {
    int stackSize = array_size(scopeStack);
    if (stackSize == 0) {
        return strdup("0"); // root scope
    }
    
    // Calculate the required buffer size
    // Each scope level needs at most 10 chars for the number, plus '.' separator
    char* scopeStr = (char*)malloc((stackSize * 11) * sizeof(char));
    if (!scopeStr) {
        fprintf(stderr, "Failed to allocate memory for scope string\n");
        exit(1);
    }
    
    scopeStr[0] = '\0';
    for (int i = 0; i < stackSize; i++) {
        int* level = (int*)array_get(scopeStack, i);
        
        // First number doesn't need a dot
        if (i == 0) {
            sprintf(scopeStr, "%d", *level);
        } else {
            char temp[12];
            sprintf(temp, ".%d", *level);
            strcat(scopeStr, temp);
        }
    }
    
    return scopeStr;
}

// Check if two scopes conflict (one is a prefix of the other or they're the same)
bool ScopesConflict(const char* scope1, const char* scope2) {
    // Scopes conflict if they are the same
    if (strcmp(scope1, scope2) == 0) {
        return true;
    }
    
    // Check if one is a prefix of the other
    size_t len1 = strlen(scope1);
    size_t len2 = strlen(scope2);

    if (len1 < len2) {
        if (strncmp(scope1, scope2, len1) == 0 && scope2[len1] == '.') {
            return true;
        }
    }

    if (len2 < len1) {
        if (strncmp(scope2, scope1, len2) == 0 && scope1[len2] == '.') {
            return true;
        }
    }
    
    return false;
}

void CleanupScopeStack() {
    if (scopeStack) {
        // Free the stack itself
        array_free(scopeStack);
        scopeStack = NULL;
    }
    
    if (scopeCounters) {
        free(scopeCounters);
        scopeCounters = NULL;
    }
}

bool isNumeric(ASTNodeType type) {
    return type == AST_INTEGER || type == AST_FLOAT;
}

void ProcessExpression(ASTNode *ctx, Array *symbol_table) {
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
            ProcessOperation(ctx, symbol_table);
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

void ProcessDeclaration(ASTNode *ctx, Array *symbol_table) {
    assert(ctx->count == 2); // ensure 2 children
    
    // get the identifier node to access the name
    ASTNode *identifierNode = &CHILD_ITEM(ctx, 1);
    printf("Declaration Analyzing -> %s\n", ASTNodeType_to_string(CHILD_TYPE(ctx, 0)));
    
    // Get the current scope
    char *currentScope = GetCurrentScope();
    
    // check for redeclaration
    bool redeclared = false;
    
    // loop through symbol table to check for redeclaration
    for (int i = 0; i < array_size(symbol_table); i++){
        symEntry *other = NULL;
        other = (symEntry *)array_get(symbol_table, i);
        
        // check if names match
        if (strcmp(other->symNode->token.lexeme, identifierNode->token.lexeme) == 0) {
            // check scopes
            if (ScopesConflict(currentScope, other->scope)) {
                redeclared = true;
                // Save Error - Redeclaration
                identifierNode->error = AST_ERROR_REDECLARATION_VAR;
                printf("Error: Variable '%s' redeclared in conflicting scope.\n", 
                       identifierNode->token.lexeme);
                break;
            }
        }
    }
    
    // if no redeclaration issue, add to symbol table
    if (!redeclared) {
        symEntry *entry = malloc(sizeof(symEntry));
        if (!entry) {
            fprintf(stderr, "Memory allocation failed for symbol table entry\n");
            free(currentScope);
            exit(1);
        }
        
        entry->scope = currentScope;  // Transfer ownership of the string
        entry->symNode = &CHILD_ITEM(ctx, 1);
        entry->type = CHILD_TYPE(ctx, 0);
        array_push(symbol_table, (Element *)entry);
        
        printf("Added '%s' to symbol table in scope '%s'\n", 
               entry->symNode->token.lexeme, entry->scope);
    } else {
        // Free currentScope if we don't store it
        free(currentScope);
    }
}

ASTNodeType returnType(ASTNode *ctx) {
    if (ctx->count == 1) return ctx->type;
    ASTNodeType leftType = CHILD_TYPE(ctx, 0);
    ASTNodeType rightType = CHILD_TYPE(ctx, 1);
    assert(leftType == rightType);
    return leftType;
}
// Need to fix multiple operators to check for types

void ProcessOperator(ASTNode *ctx, Array *symbol_table){
    assert(ctx->count == 2); // Binary operator
    ASTNodeType leftType = CHILD_TYPE(ctx, 0);
    ASTNodeType rightType = CHILD_TYPE(ctx, 1);
    if (isNumeric(leftType) && isNumeric(rightType)) assert(leftType == rightType); // ctx->type = returnType(ctx);
    printf("Operator Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    ProcessExpression(&CHILD_ITEM(ctx, 0), symbol_table);
    ProcessExpression(&CHILD_ITEM(ctx, 1), symbol_table);
}

void ProcessUnaryOperator(ASTNode *ctx, Array *symbol_table){
    assert(ctx->count == 1);
    printf("Operator Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    ProcessExpression(&CHILD_ITEM(ctx, 0), symbol_table);
}

void ProcessFunction(ASTNode *ctx, Array *symbol_table){
    printf("Function Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
    assert(ctx->count == 1);
    ProcessExpression(ctx->items, symbol_table);

}

// Handles Assignment, Operator, Unary Operator
void ProcessOperation(ASTNode *ctx, Array *symbol_table){
    if (ctx->type == AST_ASSIGN_EQUAL) {
        printf("Assignment Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));
        assert(ctx->count == 2);
        assert(CHILD_TYPE(ctx, 0) == AST_IDENTIFIER); // LHS must be an identifier
        assert(CHILD_TYPE(ctx, 1) != AST_ASSIGN_EQUAL); // prevent chained assignment
        ProcessExpression(&CHILD_ITEM(ctx, 1), symbol_table); // recurse because RHS expression

    } else if (ctx->type >= AST_LOGICAL_OR && ctx->type < AST_BITWISE_NOT) {
        ProcessOperator(ctx, symbol_table);

    } else if (ctx->type >= AST_BITWISE_NOT) {
        ProcessUnaryOperator(ctx, symbol_table);
    }
}

int currScope;

void ProcessScope(ASTNode *ctx, Array *symbol_table) {
    assert(ctx->type == AST_SCOPE);
    
    EnterScope();  // Enter a new scope when processing a scope node
    char* currentScope = GetCurrentScope();
    printf("\nEntered new scope -> %s\n", currentScope);
    free(currentScope);  // Free the string after using it
    
    for (size_t child = 0; child < ctx->count; child++) {
        ASTNode *childNode = &ctx->items[child];
        ProcessNode(childNode, symbol_table);
    }
    
    ExitScope();  // Exit the scope when done processing
    if (array_size(scopeStack) > 0) {
        currentScope = GetCurrentScope();
        printf("Returned to scope -> %s\n", currentScope);
        free(currentScope);
    }
}

void ProcessConditional(ASTNode *ctx, Array *symbol_table) { // If statements
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

    ProcessOperation(&CHILD_ITEM(ctx, 0), symbol_table); // Process the comparison
    ProcessScope(&CHILD_ITEM(ctx, 1), symbol_table); // ThenScope
    ProcessScope(&CHILD_ITEM(ctx, 2), symbol_table); // ElseScope
}

void ProcessLoop(ASTNode *ctx, Array *symbol_table) {
    assert(ctx->type == AST_WHILE_LOOP || ctx->type == AST_REPEAT_UNTIL_LOOP);
    assert(ctx->count == 2);

    printf("Loop Analyzing -> %s\n", ASTNodeType_to_string(ctx->type));

    if (ctx->type == AST_WHILE_LOOP) {
        ProcessExpression(&CHILD_ITEM(ctx, 0), symbol_table);
        ProcessScope(&CHILD_ITEM(ctx, 1), symbol_table);
    }

    if(ctx->type == AST_REPEAT_UNTIL_LOOP) {
        ProcessScope(&CHILD_ITEM(ctx, 0), symbol_table);
        ProcessExpression(&CHILD_ITEM(ctx, 1), symbol_table);
    }
}

// Only export
void ProcessNode(ASTNode *ctx, Array *symbol_table) {
    switch(ctx->type) {
        case AST_SCOPE:
        ProcessScope(ctx, symbol_table);
        break;
        case AST_PROGRAM:
        ProcessProgram(ctx, symbol_table);
        break;
        case AST_CODITIONAL:
        ProcessConditional(ctx, symbol_table);
        break;
        case AST_REPEAT_UNTIL_LOOP:
        case AST_WHILE_LOOP:
        ProcessLoop(ctx, symbol_table);
        break;
        case AST_EXPRESSION:
        ProcessExpression(ctx, symbol_table);
        break;
        case AST_DECLARATION:
        ProcessDeclaration(ctx, symbol_table);
        break;
        case AST_READ:
        case AST_PRINT:
        ProcessFunction(ctx, symbol_table);
        break;
    }
}

void ProcessProgram(ASTNode *head, Array *symbol_table) {
    assert(head->type == AST_PROGRAM);
    printf("\nStarting Semantic Analysis:\n");
    
    // Initialize the scope tracking system
    InitializeScopeStack();
    
    ProcessNode(head->items, symbol_table);
    
    // Clean up the scope tracking system
    CleanupScopeStack();
}