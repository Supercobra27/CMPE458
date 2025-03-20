// Initialize with "0"
static char currentScope[100] = "0";  

// Modified EnterScope function
void EnterScope(int scopeNumber) {
    char temp[100];
    
    if (strlen(currentScope) == 0) {
        // First scope level - just use the number
        sprintf(temp, "%d", scopeNumber);
    } else {
        // Add a dot and the new scope number
        sprintf(temp, "%s.%d", currentScope, scopeNumber);
    }
    
    strcpy(currentScope, temp);
}

// ExitScope stays mostly the same
void ExitScope() {
    char *lastDot = strrchr(currentScope, '.');
    if (lastDot != NULL) {
        *lastDot = '\0';  // Remove everything after the last dot
    } else {
        // No dots left, so we're back to root
        currentScope[0] = '\0';  // Return to empty string
    }
} 