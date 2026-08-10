#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum {
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


#endif