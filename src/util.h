#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


// OPERATORS
// repr char for easy printing
typedef enum {
    OPERATOR_ADD = '+',
    OPERATOR_SUB = '-',
    OPERATOR_MUL = '*',
    OPERATOR_DIV = '/',
    OPERATOR_MOD = '%',
} BinaryOperator;

typedef enum {
    OPERATOR_BITWISE_COMPLEMENT = '~',
    OPERATOR_NEGATE = '-'
} UnaryOperator;


typedef struct {
    char* ptr;
    size_t len;
    size_t cap;
} String;

String String_new(size_t cap);
void String_push(String* string, const char* str2);
void String_push_format(String* string, size_t required_len, const char* fmt, ...);

#endif