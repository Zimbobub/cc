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

    // 1 char
    Semicolon,
    Period,
    Comma,

    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,

    BitwiseNot,
    Minus,

    // 2 char
    Increment,
    Decrement
} TokenType;

typedef struct {
    TokenType type;
    char* src;
    const char* file_name;
    size_t line_num;
    size_t col_num;
} Token;

typedef struct {
    Token* tokens;
    size_t n_tokens;
} TokenBuf;

const char* get_token_name(TokenType type);
void print_token(Token *token);
void print_tokens(TokenBuf *tokens);
void tokenbuf_destruct(TokenBuf* this);

#endif