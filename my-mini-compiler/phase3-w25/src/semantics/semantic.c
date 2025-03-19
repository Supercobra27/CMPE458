#include "semantic.h"

void ProcessDeclaration(ASTNode *ctx) // Simon
{
    //check that node exists and has children
    if(ctx == NULL || ctx->count != 2)
    {
        return; // return error by declaration
    }

    char *name = ctx->items[1].token.lexeme;
    //get current scope
    char *scope = 

    //check that that name is not already declared in the current scope
    //TODO

    //add name to symbol table
    //TODO
}

void ProcessFunction(ASTNode *ctx) // Simon
{
    // factrorial, prrint, read, anything else?
    // print and read are keyword, expression, end
    //factorial is keyword, paren, expression, paren, null 

    //check that node exists and has children
    if(ctx == NULL || ctx->count > 0)
    {
        return; // return error in declaration
    }

    // do we need to check that the keyword is valid or can we assume it's valid?
    //how are these rules getting called? are we just calling this rule when needed, ie when we see a correct keyword?

    //process expression
    // if not factorial process 2nd element
    // if factorial process 3rd element
    if(ctx->items[0].type == AST_FACTORIAL)
        ProcessExpression(&ctx->items[2]);

    else
        ProcessExpression(&ctx->items[1]);

    //sort of like with checking the keyword do we need to check that the parenthases exists? 



}

void ProcessScope(ASTNode *ctx) // Simon
{
    //rename this to block? makes more sense to me to have coinsistent naming

    //check that node exists and has children
    if(ctx == NULL || ctx->count < 1)
    {
        return; // return error by declaration
    }

    //is there anything to process or is it just step the block in the right direction?

    //maybe step scope forwards, process the internals, step the scope back

}
void ProcessString(ASTNode *ctx) // Simon
{
    //check that node exists and has children
    if(ctx == NULL || ctx->count != 1)
    {
        return; // return error by declaration
    }

    //do we need ot check if the string is valid? is that already done at the lexer?

    //add string to symbol table
    
}