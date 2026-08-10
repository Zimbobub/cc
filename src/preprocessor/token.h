#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

enum TokenType {
    Identifier,
    Number,
    StringLiteral,

    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,

    Semicolon,
    Period,
    Comma
};

typedef struct {
    TokenType type;
    char* src;
    size_t line_num;
    size_t col_num;
} Token;


#endif