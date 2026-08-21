#include "parser.h"



void expect_token(TokenBuf tokens, size_t* i, TokenType token_type) {
    if (*i >= tokens.n_tokens) parser_error(&tokens, i, "Index out of range");
    if (tokens.tokens[*i].type != token_type) {
        parser_error(&tokens, i, "Expected %s, got %s", get_token_name(token_type), get_token_name(tokens.tokens[*i].type));
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
        parser_error(&tokens, i, "Expected keyword '%s', got '%s'", keyword, tokens.tokens[(*i)-1].src);
    }
}



CExpression parse_expression(TokenBuf tokens, size_t* i) {
    printf("parse expression\n");
    TokenType next = tokens.tokens[*i].type;
    if (next == Number) {
        // constant
        int val = expect_number(tokens, i);
        CExpression expr = {
            EXPRESSION_CONST,
            .expr.constant = { .val = val }
        };
        return expr;
    } else if (next == LParen) {
        // expr in parentheses
        expect_token(tokens, i, LParen);
        CExpression expr = parse_expression(tokens, i);
        expect_token(tokens, i, RParen);
        return expr;
    } else if (next == Tilde || next == Minus) {
        // unary operator
        UnaryOperator op = OPERATOR_BITWISE_COMPLEMENT;
        if (next == Tilde) {
            expect_token(tokens, i, Tilde);
            op = OPERATOR_BITWISE_COMPLEMENT;
        } else if (next == Minus) {
            expect_token(tokens, i, Minus);
            op = OPERATOR_SUB;
        }

        CExpression* inner = malloc(sizeof(CExpression));
        if (inner == NULL) parser_error(&tokens, i, "malloc failed");
        *inner = parse_expression(tokens, i);
        CExpression expr = {
            EXPRESSION_UNARY,
            .expr.unary = {
                .op = op,
                .val = inner
            }
        };
        return expr;
    } else {
        parser_error(&tokens, i, "unknown expression token (got %s)", get_token_name(next));
    }
}

CStatement parse_statement(TokenBuf tokens, size_t* i) {
    printf("parse statement\n");
    expect_keyword(tokens, i, "return");
    CExpression expr = parse_expression(tokens, i);
    expect_token(tokens, i, Semicolon);
    CStatement statement = {
        .type=STATEMENT_RETURN,
        .statement = { .ret = expr }
    };
    return statement;
}

CFunctionDefinition parse_function_definition(TokenBuf tokens, size_t* i) {
    printf("parse function definition\n");
    expect_keyword(tokens, i, "int");

    char* name = expect_identifier(tokens, i);
    expect_token(tokens, i, LParen);
    expect_keyword(tokens, i, "void");
    expect_token(tokens, i, RParen);

    expect_token(tokens, i, LBrace);    
    CStatement statement = parse_statement(tokens, i);
    expect_token(tokens, i, RBrace);

    CFunctionDefinition func = { name, statement };
    return func;
}

CProgram parse_program(TokenBuf tokens) {
    printf("parse program\n");
    size_t i = 0;
    CFunctionDefinition func = parse_function_definition(tokens, &i);

    // expect no more tokens
    if (i < tokens.n_tokens) parser_error(&tokens, &i, "Unexpected token at end of file");

    CProgram program = { func };
    return program;
}
