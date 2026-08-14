#include "parser.h"


void throw_err_at_token(TokenBuf tokens, size_t* i, const char* msg) {
    printf("Parser error at %ld:%ld\n", tokens.tokens[*i].line_num, tokens.tokens[*i].col_num);
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}


void expect_token(TokenBuf tokens, size_t* i, TokenType token_type) {
    if (*i >= tokens.n_tokens) throw_err_at_token(tokens, i, "Index out of range");
    if (tokens.tokens[*i].type != token_type) {
        char* msg[50] = {0}; // token type names dont get any larger than 15 char
        snprintf(msg, 49, "Expected %s, got %s", get_token_name(token_type), get_token_name(tokens.tokens[*i].type));
        throw_err_at_token(tokens, i, msg);
    }
    (*i)++;
}

int expect_number(TokenBuf tokens, size_t* i) {
    expect_token(tokens, i, Number);
    return atoi(tokens.tokens[(*i)-1].src);
}

char* expect_identifier(TokenBuf tokens, size_t* i) {
    expect_token(tokens, i, Identifier);
    return tokens.tokens[(*i)-1].src;
}

void expect_keyword(TokenBuf tokens, size_t* i, const char* keyword) {
    expect_token(tokens, i, Keyword);
    if (strcmp(tokens.tokens[(*i)-1].src, keyword) != 0) {
        char* msg[50] = {0}; // token type names dont get any larger than 15 char
        snprintf(msg, 49, "Expected keyword '%s', got '%s'", keyword, tokens.tokens[(*i)-1].src);
        throw_err_at_token(tokens, i, msg);
    }
}



Expression parse_expression(TokenBuf tokens, size_t* i) {
    printf("parse expression\n");
    int val = expect_number(tokens, i);
    Expression expr = { EXPRESSION_CONST, 0 };
    expr.expr.constant.val = val;
    return expr;
}

Statement parse_statement(TokenBuf tokens, size_t* i) {
    printf("parse statement\n");
    expect_keyword(tokens, i, "return");
    Expression expr = parse_expression(tokens, i);
    expect_token(tokens, i, Semicolon);
    Statement statement = {
        STATEMENT_RETURN,
        0
    };
    statement.statement.ret = expr;
    return statement;
}

FunctionDefinition parse_function_definition(TokenBuf tokens, size_t* i) {
    printf("parse function definition\n");
    expect_keyword(tokens, i, "int");

    char* name = expect_identifier(tokens, i);
    expect_token(tokens, i, LParen);
    expect_keyword(tokens, i, "void");
    expect_token(tokens, i, RParen);

    expect_token(tokens, i, LBrace);    
    Statement statement = parse_statement(tokens, i);
    expect_token(tokens, i, RBrace);

    FunctionDefinition func = { name, statement };
    return func;
}

Program parse_program(TokenBuf tokens) {
    printf("parse program\n");
    size_t i = 0;
    FunctionDefinition func = parse_function_definition(tokens, &i);

    // expect no more tokens
    if (i < tokens.n_tokens) throw_err_at_token(tokens, &i, "Unexpected token at end of file");

    Program program = { func };
    return program;
}
