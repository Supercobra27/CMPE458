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
    bool grammar_check_verbose;
    bool show_input;
    bool print_tokens;
    bool print_parse_tree;
    bool print_abstract_syntax_tree;
    bool print_symbol_table; 
} const DEBUG = {
    .grammar_check = true,
    .grammar_check_verbose = false,
    .show_input = false,
    .print_tokens = false, 
    .print_parse_tree = false, 
    .print_abstract_syntax_tree = false, 
    .print_symbol_table = false
};
// File extension for input files
const char *const FILE_EXT = ".cisc";


/**
 * Print Token information to stdout.
 * 
 * Prints the type, lexeme, line, col_start, col_end, and error message of the token.
 */
void print_token(Token token)
{
    printf("Token type=%-10s(%d), lexeme=\"%s\", line=%-2d, column:%d-%d, error_message=\"%s\"",
           TokenType_to_string(token.type), token.type, token.lexeme, token.position.line, token.position.col_start, token.position.col_end, ErrorType_to_error_message(token.error));
}

/**
 * WARNING: this function does not check if the pointer is NULL.
 * @return n->children.
 */
ParseTreeNode *ParseTreeNode_children_begin(const ParseTreeNode *const n) {
    return n->children;
}
/**
 * WARNING: this function does not check if the pointer is NULL.
 * @return n->count.
 */
size_t ParseTreeNode_num_children(const ParseTreeNode *const n) {
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
const ASTNode *ASTNode_children_begin(const ASTNode *const n) {
    return n->items;
}
size_t ASTNode_num_children(const ASTNode *const n) {
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

void print_token_compiler_message(FILE *const stream, const Lexer *const l, const char *input_file_path, const Token *const token, const char *const error_message)
{
    const int line_start_pos = *(int *)array_get(l->line_start_positions, token->position.line - 1);
    const char *const line_end = strchr(l->input_string + line_start_pos, '\n');
    const int line_length = line_end == NULL ? (int)strlen(l->input_string + line_start_pos) : line_end - (l->input_string + line_start_pos);
    // tildes is supposed to be as long as the longest token lexeme so that it can always be chopped to the right length.
    static const char *tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    fprintf(stream,
        "%s:%d:%d: %s\n"
        "%.*s\n"
        "%*s%.*s\n",
        input_file_path, token->position.line, token->position.col_start, error_message,
        line_length, l->input_string + line_start_pos,
        token->position.col_start, "^", token->position.col_end - token->position.col_start, tildes);
}


// Enhanced syntax error reporting function using new print function
void report_syntax_errors(FILE *const stream, const Lexer *const l, const ParseTreeNode *const node, const char *const filepath) {
    // print error message if the node has an error and it has a token that was not already reported as an error by the lexer
    switch (node->error) {
        case PARSE_ERROR_NONE:
        case PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE:
            break;
        case PARSE_ERROR_CHILD_ERROR:
            for (const ParseTreeNode *child = node->children; child != node->children + node->count; ++child) {
                report_syntax_errors(stream, l, child, filepath);
            }
            break;
        case PARSE_ERROR_NO_RULE_MATCHES:
        case PARSE_ERROR_WRONG_TOKEN:
            if (node->token) {
                const unsigned int MESSAGE_SIZE = 100;
                char *message = malloc(MESSAGE_SIZE);
                snprintf(message, MESSAGE_SIZE, "error: expected a %s", ParseToken_to_string(node->type));
                print_token_compiler_message(stream, l, filepath, node->token, message);
                free(message);
            }
            break;
    }
}


int main(int const argc, const char *const argv[]) {
    if (DEBUG.grammar_check) {
        if (DEBUG.grammar_check_verbose)
            printf("Validating grammar:\n");
        CFG_GrammarCheckResult result = check_cfg_grammar(DEBUG.grammar_check_verbose ? stdout : NULL, program_grammar);
        if (result.missing_or_mismatched_rules
            || result.contains_improperly_terminated_production_rules
            || !result.is_prefix_free 
            || result.contains_direct_left_recursive_rule_as_last_rule 
            || result.contains_indirect_left_recursion) {
            fprintf(stderr, "Grammar is invalid, compilation cannot be proceed.\n");
            return -1;  // early return
        }
        if (DEBUG.grammar_check_verbose)
            printf("Grammar is valid.\n");
    }
    
    const char *input_file_path = NULL;
    char *input = NULL;
    bool must_free_input = false;
    if (argc == 2) {
        // Input file extension check
        const int file_path_len = strlen(argv[1]);
        input_file_path = argv[1];
        if (file_path_len < 5 || strncmp(input_file_path + file_path_len - 5, FILE_EXT, 5) != 0) {
            fprintf(stderr, "Incorrect file extension, the correct extension is .cisc\n");
            return -1;
        }
        // Load file into input.
        FILE *file = fopen(input_file_path, "r");
        if (file == NULL) {
            fprintf(stderr, "Error: Unable to open file %s\n", input_file_path);
            return -1;
        }
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        input = malloc(file_size + 1);
        if (input == NULL) {
            fprintf(stderr, "Error: Unable to allocate memory for file %s\n", input_file_path);
            return -1;
        }
        must_free_input = true;

        size_t bytes_read = 0;
        char ch;
        while (bytes_read < file_size && (ch = fgetc(file)) != EOF && ch != '\0')
            input[bytes_read++] = ch;
        input[bytes_read] = '\0';
        fclose(file);
    }
    else {
        input_file_path = "<input-file-name>.cisc";
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

        // Print usage for file input.
        printf(
            "Usage: .\\my-mini-compiler.exe %s\n"
            "Using default input:\n"
            "```\n%s\n```\n", 
            input_file_path, input);
    }

    if (DEBUG.show_input)
        printf("\nProcessing input:\n```\n%s\n```", input);

    // Tokenize the input
    if (DEBUG.print_tokens) 
        printf("\nTokenizing:\n");
    Array *tokens = array_new(8, sizeof(Token));
    Lexer l = {0};
    init_lexer(&l, input, 0);
    Token token;
    do {
        token = get_next_token(&l);
        array_push(tokens, (Element *)&token);
        if (token.error != ERROR_NONE)
            print_token_compiler_message(stderr, &l, input_file_path, &token, ErrorType_to_error_message(token.error));
        if (DEBUG.print_tokens) {
            print_token(token); 
            printf("\n");
        }
    } while (token.type != TOKEN_EOF);

    // Parse the input
    size_t token_index = 0;
    ParseTreeNode pt_root; pt_root.type = PT_PROGRAM;
    parse_cfg_recursive_descent_parse_tree(&pt_root, &token_index, (Token *)array_begin(tokens), program_grammar, ParseToken_COUNT_NONTERMINAL);
    // currently, the parser does not perform error recovery, so at most one syntax error can be reported.
    report_syntax_errors(stderr, &l, &pt_root, input_file_path);
    if (DEBUG.print_parse_tree) {
        printf("\nParse Tree:\n");
        print_tree(&(print_tree_t){
            .root = &pt_root,
            .children = (const_voidp_to_const_voidp*)ParseTreeNode_children_begin,
            .count = (const_voidp_to_size_t*)ParseTreeNode_num_children,
            .size = sizeof(ParseTreeNode),
            .print_head = (const_voidp_to_void*)ParseTreeNode_print_head,
        });
    }

    // Convert to Abstract Syntax Tree
    ASTNode ast_root; ast_root.type = AST_PROGRAM;
    ASTNode_from_ParseTreeNode(&ast_root, (ParseTreeNodeWithPromo *)&pt_root);
    if (DEBUG.print_abstract_syntax_tree) {
        printf("\nAbstract Syntax Tree:\n");
        print_tree(&(print_tree_t){
            .root = &ast_root,
            .children = (const_voidp_to_const_voidp*)ASTNode_children_begin,
            .count = (const_voidp_to_size_t*)ASTNode_num_children,
            .size = sizeof(ASTNode),
            .print_head = (const_voidp_to_void*)ASTNode_print_head,
        });
    }

    // TODO: Semantic Analysis

    // TODO: print semantic errors if any.

    ParseTreeNode_free_children(&pt_root);
    ASTNode_free_children(&ast_root);
    array_free(tokens);
    if (must_free_input)
        free(input);
    return 0;
}
