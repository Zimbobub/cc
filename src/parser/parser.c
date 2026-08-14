#include "parser.h"


// void throw_err_at_token(TokenBuf tokens, size_t* i, const char* msg) {
//     printf("")
// }


bool expect_token(TokenBuf tokens, size_t* i, TokenType token_type) {
    if (*i >= tokens.n_tokens) return false;
    if (tokens.tokens[*i].type != token_type) return false;
    ++i;
    return true;
}

bool expect_keyword_token(TokenBuf tokens, size_t* i, const char* keyword) {
    if (*i >= tokens.n_tokens) return false;
    if (tokens.tokens[*i].type != Keyword) return false;
    if (strcmp(tokens.tokens[*i].src, keyword) != 0) return false;
    ++i;
    return true;
}

Expression* parse_expression(TokenBuf tokens, size_t* i) {
    
}

Statement* parse_statement(TokenBuf tokens, size_t* i) {
    
}

FunctionDefinition* parse_function_definition(TokenBuf tokens, size_t* i) {
    if (!expect_keyword_token(tokens, i, "int")) return NULL;
    if (!expect_token(tokens, i, LParen)) return NULL;
    char* name = tokens.tokens[*i].src;
    Statement* statement = parse_statement(tokens, i);
    FunctionDefinition func = {name, statement};
    return func;
}

Program parse_program(TokenBuf tokens) {
    FunctionDefinition* func = parse_function_definition(tokens, 0);
    Program program = { func };
    return program;
}
