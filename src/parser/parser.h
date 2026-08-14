#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../lexer/token.h"
#include "ast.h"

typedef struct {
    TokenBuf tokens;
    Program* ast;
} Parser;

Parser parser_init(TokenBuf tokens);
bool parser_run(Parser* this);

#endif