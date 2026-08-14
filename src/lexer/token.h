#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    Unknown,
    
    Keyword,
    Identifier,
    Number,
    StringLiteral,

    Semicolon,
    Period,
    Comma,

    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace
} TokenType;

typedef struct {
    TokenType type;
    char* src;
    size_t line_num;
    size_t col_num;
} Token;

typedef struct {
    Token* tokens;
    size_t n_tokens;
} TokenBuf;

char* get_token_name(TokenType type);
void print_token(Token *token);
void tokenbuf_destruct(TokenBuf* this);

#endif