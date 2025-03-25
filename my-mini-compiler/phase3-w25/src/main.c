#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/dynamic_array.h"
#include "../include/grammar.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/tree.h"

// Debugging flag
struct debug_flags {
    bool grammar_check;
    bool show_input;
    bool print_tokens;
    bool print_parse_tree;
    bool print_abstract_syntax_tree;
    bool print_symbol_table; 
} const DEBUG = {
    .grammar_check = false,
    .show_input = false,
    .print_tokens = false, 
    .print_parse_tree = false, 
    .print_abstract_syntax_tree = false, 
    .print_symbol_table = false
};
// File extension for input files
const char *const FILE_EXT = ".cisc";

/**
 * WARNING: this function does not check if the pointer is NULL.
 * @return n->children.
 */
ParseTreeNode *ParseTreeNode_children_begin(const ParseTreeNode *n) {
    return n->children;
}
/**
 * WARNING: this function does not check if the pointer is NULL.
 * @return n->count.
 */
size_t ParseTreeNode_num_children(const ParseTreeNode *n) {
    return n->count;
}
/**
 * Print ParseTreeNode to stdout. Prints the type and error types of the node, then the same for the token (if the token is not null) followed by a newline.
 * 
 * WARNING: this function does not check if the pointer is NULL.
 * 
 * @param node Pointer to node to print.
 */
void ParseTreeNode_print_head(const ParseTreeNode *const node) {
    printf("%s", ParseToken_to_string(node->type));
    if (node->error) 
        printf(" (%s)", ParseErrorType_to_string(node->error));
    if (node->token)
    {
        printf(" -> ");
        if (node->error || node->token->error)
            print_token(*node->token);
        else
            printf("%s \"%s\"", TokenType_to_string(node->token->type), node->token->lexeme);
    }
}
const ASTNode *ASTNode_children_begin(const ASTNode *n) {
    return n->items;
}
size_t ASTNode_num_children(const ASTNode *n) {
    return n->count;
}
/**
 * Print ASTNode to stdout. Prints the type and error types of the node, then the same for the token (if the token is not null) followed by a newline.
 * 
 * WARNING: this function does not check if the pointer is NULL.
 * 
 * @param node Pointer to node to print.
 */
void ASTNode_print_head(const ASTNode *const node) {
    printf("%s", ASTNodeType_to_string(node->type));
    if (node->error) 
        printf(" (%s)", ASTErrorType_to_string(node->error));
    if (node->token.type != TOKEN_NULL)
    {
        printf(" -> ");
        if (node->error || node->token.error)
            print_token(node->token);
        else
            printf("%s \"%s\"", TokenType_to_string(node->token.type), node->token.lexeme);
    }
}


// Global variables used for syntax error handling
extern const char *global_input;
extern Array *global_line_start;

// Function to handle syntax errors similarly to lexical errors
void print_syntax_compiler_message(const char *input_file_path, const Token *token, ParseErrorType error) {
    const int line_start_pos = *(int *)array_get(global_line_start, token->position.line - 1);
    const char *const line_end = strchr(global_input + line_start_pos, '\n');
    const int line_length = line_end == NULL ? (int)strlen(global_input + line_start_pos) : line_end - (global_input + line_start_pos);
    static const char *tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    printf(
            "%s:%d:%d: %s\n"
            "%.*s\n"
            "%*s%.*s\n",
            input_file_path, token->position.line, token->position.col_start, ParseErrorType_to_string(error),
            line_length, global_input + line_start_pos,
            token->position.col_start, "^", token->position.col_end - token->position.col_start, tildes);
}

// Enhanced syntax error reporting function using new print function
void report_syntax_errors(const ParseTreeNode *node, const char *filepath) {
    if (node->error != PARSE_ERROR_NONE && node->token) {
        print_syntax_compiler_message(filepath, node->token, node->error);
    }
    for (size_t i = 0; i < node->count; i++) {
        report_syntax_errors(&node->children[i], filepath);
    }
}



// Main function for testing
int main(int argc, char *argv[])
{
    // setbuf(stdout, NULL);  // Disable buffering 
    printf("Validating grammar:\n");
    CFG_GrammarCheckResult result = check_cfg_grammar(DEBUG.grammar_check ? stdout : NULL, program_grammar);
    if (result.missing_or_mismatched_rules
        || result.contains_improperly_terminated_production_rules
        || !result.is_prefix_free 
        || result.contains_direct_left_recursive_rule_as_last_rule 
        || result.contains_indirect_left_recursion
    )
    {
        printf("Grammar is invalid, cannot be parsed correctly.\n");
        return -1;  // early return
    }
    printf("Grammar is valid.\n");
    
    // input
    char *input_file_path = NULL;
    char *input = NULL;
    bool must_free_input = false;
    if (argc == 2)
    {
        // Input file extension check
        int file_len = strlen(argv[1]);
        char *file_name = argv[1];
        if (file_len < 5 || strncmp(file_name + file_len - 5, FILE_EXT, 5) != 0)
        {
            printf("Incorrect file extension, the correct extension is .cisc");
            return -1;
        }
        // Load file into input.
        FILE *file = fopen(file_name, "r");
        if (file == NULL)
        {
            printf("Error: Unable to open file %s\n", file_name);
            return -1;
        }
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        input = malloc(file_size + 1);
        if (input == NULL)
        {
            printf("Error: Unable to allocate memory for file %s\n", file_name);
            return -1;
        }
        must_free_input = true;

        size_t bytes_read = 0;
        char ch;
        while (bytes_read < file_size && (ch = fgetc(file)) != EOF && ch != '\0')
            input[bytes_read++] = ch;
        input[bytes_read] = '\0';
        fclose(file);
        input_file_path = file_name;
    }
    else
    {
        // Print usage for file input.
        printf("\nUsage: .\\my-mini-compiler.exe <Input-File-Name>.cisc\n");
        input_file_path = "<Input-File-Name>.cisc";
        // Test with both valid and invalid inputs
        // input = "int x;\n"   // Valid declaration
        //         "x = 42;\n"; // Valid assignment;
        input = "int x;\n"
                "x = 42;\n"
                "int ?;\n"
                "string @#;\n";

        // input = 
        //     "{ \n    float w;\n    w = 3.14159;\n    {{{{\"middle\";}}}}\n}\n"
        //     "; ?? skip the empty statement\n"
        //     "string s;\n"
        //     "1 + 2 + 3;\n"
        //     "x = y = z + 1 - 1;\n"
        //     "print 5 * (2 + 3);\n"
        //     "read x;\n"
        //     "if 1 then { }\n"
        //     "if 1 then { } else { x = 1; }\n"
        //     "while 0 { }\n"
        //     "repeat { } until 1;\n";
    }

    printf("\nProcessing input:\n```\n%s\n```", DEBUG.show_input ? input : "Not showing input.");

    Array *tokens = array_new(8, sizeof(Token));
    // Tokenize the input
    printf("\nTokenizing:\n");
    init_lexer(input, 0);
    Token token = (Token){
        .error = ERROR_NONE, 
        .type = TOKEN_EOF, 
        .lexeme = "", 
        .position = (LexemePosition){.line = 0, .col_end = 0, .col_start = 0},};
    do
    {
        token = get_next_token();

        if (token.error != ERROR_NONE) {
            print_token_compiler_message(input_file_path, token);
        }

        array_push(tokens, (Element *)&token);
        if (DEBUG.print_tokens) {
            print_token(token); printf("\n");
        }
    } while (token.type != TOKEN_EOF);

    // Parse the input
    size_t token_index = 0;
    ParseTreeNode root = (ParseTreeNode){
        .type = ParseToken_START_NONTERMINAL,
        .token = NULL,
        .rule = NULL,
        .finalized_promo_index = SIZE_MAX,
        .error = PARSE_ERROR_NONE,
        .children = NULL,
        .capacity = 0,
        .count = 0,
    };
    parse_cfg_recursive_descent_parse_tree(&root, &token_index, (Token *)array_begin(tokens), program_grammar, ParseToken_COUNT_NONTERMINAL);

    // Call this after parsing to print syntax errors
    report_syntax_errors(&root, input_file_path);


    if (DEBUG.print_parse_tree) print_tree(&(print_tree_t){
        .root = &root,
        .children = (const_voidp_to_const_voidp*)ParseTreeNode_children_begin,
        .count = (const_voidp_to_size_t*)ParseTreeNode_num_children,
        .size = sizeof(ParseTreeNode),
        .print_head = (const_voidp_to_void*)ParseTreeNode_print_head,
    });

    // Convert to Abstract Syntax Tree
    printf("\nAbstract Syntax Tree:\n");
    ASTNode ast_root; ast_root.type = AST_PROGRAM;
    ASTNode_from_ParseTreeNode(&ast_root, (ParseTreeNodeWithPromo *)&root);

    if (DEBUG.print_abstract_syntax_tree) print_tree(&(print_tree_t){
        .root = &ast_root,
        .children = (const_voidp_to_const_voidp*)ASTNode_children_begin,
        .count = (const_voidp_to_size_t*)ASTNode_num_children,
        .size = sizeof(ASTNode),
        .print_head = (const_voidp_to_void*)ASTNode_print_head,
    });

    // TODO: Semantic Analysis

    // TODO: print Semantic compiler error if any.

    ParseTreeNode_free_children(&root);
    array_free(tokens);
    if (must_free_input)
        free(input);
    return 0;
}
