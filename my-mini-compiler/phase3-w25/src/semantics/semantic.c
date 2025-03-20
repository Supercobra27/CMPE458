#include "semantic.h"
#include <assert.h>

void ProcessDeclaration(ASTNode *ctx) // Simon
{
    assert(ctx->count == 2); // ensure 2 children
    
    // get the identifier node to access the name
    ASTNode *identifierNode = &ctx->items[1];
    
    const char *currentScope = GetCurrentScope();
    
    // check for redeclaration
    bool redeclared = false;
    
    // loop through symbol table to check for redeclaration
    for (int i = 0; i < SOMETHING; i++) { // this should be looking through the dynamic array but I think something isn't clicking for me so I just put SOMETHING
        Token *other = SOMETHING[i];
        
        // check if names match
        if (strcmp(other->lexeme, identifierNode->token.lexeme) == 0) {
            // check scopes
            if (ScopesConflict(currentScope, other->scope)) {
                redeclared = true;
                // report error - redeclaration
                fprintf(stderr, "Error: Redeclaration of variable %s on line %d\n", identifierNode->token.lexeme, identifierNode->token.position);
                exit(1);
            }
        }
    }
    
    // if no redeclaration issue, add to symbol table
    if (!redeclared) {
    }
}

void ProcessFunction(ASTNode *ctx) // Simon
{
    assert(ctx->count == 2); // keyword amd expression

    ProcessExpression(&ctx->items[1]);
}


// global scope tracking
static char currentScope[100] = "1";  // does this need to start with 1?

// get the current scope
const char* GetCurrentScope() {
    return currentScope;
}

// enter a new scope
void EnterScope(int scopeNumber) {
    char temp[100];
    sprintf(temp, "%s.%d", currentScope, scopeNumber);
    strcpy(currentScope, temp);
}

// exit current scope
void ExitScope() {
    // find last dot and remove everything after it
    char *lastDot = strrchr(currentScope, '.');
    if (lastDot != NULL) {
        *lastDot = '\0';
    }
}

bool ScopesConflict(const char *scope1, const char *scope2) {
    // two scopes conflict if one is a prefix of the other
    // 1 conflicts with 1.2, but 1.2 doesn't conflict with 1.3
    
    // check if scope1 is a prefix of scope2
    size_t len1 = strlen(scope1);
    if (strncmp(scope1, scope2, len1) == 0) {
        // make sure it's a full match not 1 matching 12
        if (scope2[len1] == '.' || scope2[len1] == '\0') {
            return true;
        }
    }
    
    // check if scope2 is a prefix of scope1
    size_t len2 = strlen(scope2);
    if (strncmp(scope2, scope1, len2) == 0) {
        if (scope1[len2] == '.' || scope1[len2] == '\0') {
            return true;
        }
    }
    
    return false;
}