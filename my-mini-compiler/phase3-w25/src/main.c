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
const bool DEBUG = true;
// File extension for input files
const char *const FILE_EXT = ".cisc";

/**
 * WARNING: this function does not check if the pointer is NULL.
 * @return n->children.
 */
ParseTreeNode **ParseTreeNode_children_begin(const ParseTreeNode *const*n) {
    return (*n)->children;
}
/**
 * WARNING: this function does not check if the pointer is NULL.
 * @return n->count.
 */
size_t ParseTreeNode_num_children(const ParseTreeNode *const*n) {
    return (*n)->count;
}
/**
 * Print ParseTreeNode to stdout. Prints the type and error types of the node, then the same for the token (if the token is not null) followed by a newline.
 * 
 * WARNING: this function does not check if the pointer is NULL.
 * 
 * @param node Pointer to node to print.
 */
void ParseTreeNode_print_head(const ParseTreeNode *const*node) {
    printf("%s", parse_token_to_string((*node)->type));
    if ((*node)->error != PARSE_ERROR_NONE) 
        printf(" (%s)", parse_error_type_to_string((*node)->error));
    if ((*node)->token != NULL)
    {
        printf(" -> ");
        if ((*node)->token->type == TOKEN_ERROR || (*node)->error != PARSE_ERROR_NONE)
            print_token(*(*node)->token);
        else
            printf("%s \"%s\"", token_type_to_string((*node)->token->type), (*node)->token->lexeme);
    }
}

// Main function for testing
int main(int argc, char *argv[])
{
    // setbuf(stdout, NULL);  // Disable buffering 
    printf("Validating grammar:\n");
    CFG_GrammarCheckResult result = check_cfg_grammar(DEBUG ? stdout : NULL, program_grammar);
    if (result.missing_or_mismatched_rules
        || result.contains_improperly_terminated_production_rules
        || !result.is_prefix_free 
        || result.contains_direct_left_recursive_rule_as_last_rule 
        || result.contains_indirect_left_recursion
    )
    {
        printf("Grammar is invalid, cannot be parsed correctly.\n");
        return -1;
    }
    printf("Grammar is valid.\n");
    
    // input
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
    }
    else
    {
        // Print usage for file input.
        printf("\nUsage: .\\my-mini-compiler.exe <Input File Name>.cisc\n");
        // Test with both valid and invalid inputs
        // input = "int x;\n"   // Valid declaration
        //         "x = 42;\n"; // Valid assignment;
        // input = "int x;\n"
        //         "x = 42;\n"
        //         "int ;";

        input = "{ \n    float w;\n    w = 3.14159;\n    {{{{\"middle\";}}}}\n}\n"
                "string s;\n"
                "1 + 2 + 3;\n"
                "x = y = z + 1;\n"
                "print 5 * (2 + 3);\n"
                "read x;\n"
                "if 1 then { }\n"
                "if 1 then { } else { }\n"
                "while 0 { }\n"
                "repeat { } until 1;\n";
        // input = "{\n   float w;\n    w = 3.14159;\n}\n";
        // input = "string s;\n";
        // input = "1 + 2 + 3;\n";
        // input = "x = y = z + 1;\n";
        // input = "print 5 * (2 + 3);\n"
        // input = "read x;\n";
        // input = "if 1 then { }\n";
        // input = "if 1 then { } else { }\n";
        // input = "while 0 { }\n";
        // input = "repeat { } until 1;\n";
    }

    // TODO: Add more test cases
    printf("\nProcessing input:\n```\n%s\n```", DEBUG ? input : "Not showing input.");

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
        array_push(tokens, (Element *)&token);
        if (DEBUG)
        {
            print_token(token); printf("\n");
        }
    } while (token.type != TOKEN_EOF);
    

    // Parse the input
    printf("\nParse Tree:\n");
    size_t token_index = 0;
    ParseTreeNode *root = parse_cfg_recursive_descent_parse_tree(program_grammar, ParseToken_COUNT_NONTERMINAL, ParseToken_START_NONTERMINAL, (Token *)array_begin(tokens), &token_index);

    if (DEBUG)
        print_tree(&(print_tree_t){
            .root = &root,
            .children = (const_voidp_to_const_voidp*)ParseTreeNode_children_begin,
            .count = (const_voidp_to_size_t*)ParseTreeNode_num_children,
            .size = sizeof(ParseTreeNode*),
            .print_head = (const_voidp_to_void*)ParseTreeNode_print_head,
        });

    ParseTreeNode_free(root, 1);
    array_free(tokens);
    if (must_free_input)
        free(input);
    return 0;
}
