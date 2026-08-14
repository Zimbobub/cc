#include "parser.h"


void throw_err_at_token(TokenBuf tokens, size_t* i, const char* msg) {
    printf("Parser error at %ld:%ld\n", tokens.tokens[*i].line_num, tokens.tokens[*i].col_num);
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}


void expect_token(TokenBuf tokens, size_t* i, TokenType token_type) {
    if (*i >= tokens.n_tokens) throw_err_at_token(tokens, i, "Index out of range");
    if (tokens.tokens[*i].type != token_type) {
        char* msg = asprintf("Expected %s, got %s", get_token_name(token_type), get_token_name(tokens.tokens[*i].type));
        throw_err_at_token(tokens, i, msg);
    }
    ++i;
}

void expect_keyword_token(TokenBuf tokens, size_t* i, const char* keyword) {
    if (*i >= tokens.n_tokens) throw_err_at_token(tokens, i, "Index out of range");
    if (tokens.tokens[*i].type != Keyword) {
        char* msg = asprintf("Expected Keyword, got %s", get_token_name(tokens.tokens[*i].type));
        throw_err_at_token(tokens, i, msg);
    }
    if (strcmp(tokens.tokens[*i].src, keyword) != 0) {
        char* msg = asprintf("Expected keyword '%s', got '%s'", keyword, tokens.tokens[*i].src);
        throw_err_at_token(tokens, i, msg);
    }
    ++i;
}



Expression* parse_expression(TokenBuf tokens, size_t* i) {
    
}

Statement* parse_statement(TokenBuf tokens, size_t* i) {
    
}

FunctionDefinition* parse_function_definition(TokenBuf tokens, size_t* i) {
    expect_keyword_token(tokens, i, "int");
    expect_token(tokens, i, LParen);
    char* name = tokens.tokens[*i].src;
    Statement* statement = parse_statement(tokens, i);
    FunctionDefinition func = {name, *statement};
    return &func;
}

Program parse_program(TokenBuf tokens) {
    FunctionDefinition* func = parse_function_definition(tokens, 0);
    Program program = { func };
    return program;
}
