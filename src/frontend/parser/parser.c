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

UnaryOperator expect_unary_operator(TokenBuf tokens, size_t* i) {
    if (tokens.tokens[(*i)].type == Tilde) {
        expect_token(tokens, i, Tilde);
        return OPERATOR_BITWISE_COMPLEMENT;
    } else if (tokens.tokens[(*i)].type == Minus) {
        expect_token(tokens, i, Minus);
        return OPERATOR_NEGATE;
    }
}

BinaryOperator expect_binary_operator(TokenBuf tokens, size_t* i) {
    (*i)++;

    if (tokens.tokens[(*i)-1].type == Plus) return OPERATOR_ADD;
    else if (tokens.tokens[(*i)-1].type == Minus) return OPERATOR_SUB;
    else if (tokens.tokens[(*i)-1].type == Asterisk) return OPERATOR_MUL;
    else if (tokens.tokens[(*i)-1].type == ForwardSlash) return OPERATOR_DIV;
    else if (tokens.tokens[(*i)-1].type == Percent) return OPERATOR_MOD;
    else parser_error(&tokens, i, "Expected binary operator, got %s", get_token_name(tokens.tokens[*i].type));
}



CExpression parse_factor(TokenBuf tokens, size_t* i); // decl for circular recursive calls

// unary expressions have highest precedence, and are parsed differently
// they they are put in their own parsing function
// <exp> ::= <factor> | <exp> <binop> <exp>
CExpression parse_expression(TokenBuf tokens, size_t* i, Precedence min_precedence) {
    printf("parse expression\n");

    CExpression expr = parse_factor(tokens, i);

    // non-operator tokens have a precedence of 0, so the loop will end at end of expr
    while (precedence_from_token(tokens.tokens[*i].type) >= min_precedence) {
        Precedence prec = precedence_from_token(tokens.tokens[*i].type);

        BinaryOperator op = expect_binary_operator(tokens, i);

        // copy prev loop's expr into left
        CExpression* left = malloc(sizeof(CExpression));
        if (left == NULL) parser_error(&tokens, i, "malloc failed");
        memcpy(left, &expr, sizeof(CExpression));

        // parse right (
        CExpression* right = malloc(sizeof(CExpression));
        if (right == NULL) parser_error(&tokens, i, "malloc failed");
        *right = parse_expression(tokens, i, prec + 1); // right hand side can only have operators with higher precedence than this one

        expr = (CExpression) {
            .type=EXPRESSION_BINARY,
            .expr.binary= {
                .left=left,
                .op=op,
                .right=right
            }
        };
    }

    return expr;
}

// <factor> ::= <int> | <unop> <factor> | "(" <exp> ")"
CExpression parse_factor(TokenBuf tokens, size_t* i) {
    printf("parse factor\n");
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
        CExpression expr = parse_expression(tokens, i, 1);
        expect_token(tokens, i, RParen);
        return expr;
    } else if (next == Tilde || next == Minus) {
        UnaryOperator op = expect_unary_operator(tokens, i);

        CExpression* inner = malloc(sizeof(CExpression));
        if (inner == NULL) parser_error(&tokens, i, "malloc failed");
        *inner = parse_factor(tokens, i);
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
    CExpression expr = parse_expression(tokens, i, 1);
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
