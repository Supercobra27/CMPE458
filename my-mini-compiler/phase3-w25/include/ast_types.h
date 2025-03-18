#ifndef AST_NODE_H
#define AST_NODE_H

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
    // AST_IGNORE_CHILDREN,
    // used by grammar rules to indicate that the Token and its subtree should be ignored in this particular position in the AST. (node type can still be promoted, if not promoted, then it is ignored entirely, in either case the children are not added to the AST)
    AST_SKIP,
    // used by grammar rules to indicate that the ParseToken's children should be put in this place. Useful when the parsetoken has multiple children and you want them all on the level of the parent node in the AST. Also useful when the children may be empty and in which case nothing is added to the AST.
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

const char *ASTNodeType_to_string(ASTNodeType t);

// More errors should be determined and added by semantic analysis.
typedef enum _ASTErrorType {
    AST_ERROR_NONE,
    AST_ERROR_MISSING_TOKEN,
    AST_ERROR_TOKEN_ERROR,
    AST_ERROR_CHILD_ERROR,
    AST_ERROR_UNSPECIFIED_PRODUCTION_RULE,
    AST_ERROR_EXPECTED_PROMOTION,
} ASTErrorType;

const char *ASTErrorType_to_string(ASTErrorType t);

#endif /* AST_NODE_H */