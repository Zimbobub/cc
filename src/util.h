#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "frontend/lexer/token.h"

// OPERATORS
typedef enum {
    OPERATOR_BITWISE_COMPLEMENT = -2,
    OPERATOR_NEGATE = -1
} UnaryOperator;

typedef enum {
    OPERATOR_UNKNOWN,
    OPERATOR_ADD,
    OPERATOR_SUB,
    OPERATOR_MUL,
    OPERATOR_DIV,
    OPERATOR_MOD,

    OPERATOR_BITWISE_AND,
    OPERATOR_BITWISE_OR,
    OPERATOR_BITWISE_XOR,
    OPERATOR_LEFT_SHIFT,
    OPERATOR_RIGHT_SHIFT
} BinaryOperator;

// https://en.cppreference.com/c/language/operator_precedence (table is upside down compared to this enum)
typedef enum {
    PRECEDENCE_UNKNOWN,
    PRECEDENCE_BITWISE_OR,
    PRECEDENCE_BITWISE_XOR,
    PRECEDENCE_BITWISE_AND,
    PRECEDENCE_SHIFT,
    PRECEDENCE_ADD,
    PRECEDENCE_MUL,
    PRECEDENCE_UNARY // unused
} Precedence;

UnaryOperator unary_operator_from_token(TokenType token);
BinaryOperator binary_operator_from_token(TokenType token);
const char* operator_to_string(int operator);
Precedence precedence(int operator);
Precedence precedence_from_token(TokenType token);



typedef struct {
    char* ptr;
    size_t len;
    size_t cap;
} String;

String String_new(size_t cap);
void String_push(String* string, const char* str2);
void String_push_format(String* string, size_t required_len, const char* fmt, ...);

#endif