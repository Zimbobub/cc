#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../lexer/token.h"
#include "ast.h"

#include "../compile_error/compile_error.h"

typedef enum {
    PRECEDENCE_UNKNOWN = 0,
    PRECEDENCE_ADD = 1,
    PRECEDENCE_MUL = 2
} Precedence;

CProgram parse_program(TokenBuf tokens);

#endif